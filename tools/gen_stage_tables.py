#!/usr/bin/env python3
"""Decode stage timelines, cell waves, and phase rules from decompressed SGM1 chunks.

Reads /tmp/chunks/c2.bin .. c6.bin (chunk index 2 is stage 1) and writes
src/core/stage_data.c. Those binaries come from tools/parse_sgm1.py plus
the LZ77 unpack and are not committed.
"""
import struct
from pathlib import Path

SKIP = {0x00:3,0x02:4,0x04:3,0x06:3,0x08:3,0x0A:3,0x0C:3,0x0E:4,0x10:3,0x12:3,
        0x14:3,0x16:3,0x18:3,0x1A:3,0x1C:4,0x1E:4,0x20:4,0x22:4,0x24:4,0x26:4,0x28:4,0x2A:4}
FIXED = {
0x02:0,0x0E:2,0x0F:0,0x10:2,0x11:3,0x12:3,
0x1A:5,0x1B:6,0x1C:5,0x1D:5,0x1E:5,0x1F:6,
0x2A:6,0x2B:3,0x2C:4,0x2D:3,0x2E:3,0x2F:3,0x30:3,
0x31:2,0x32:5,0x33:5,0x34:5,0x35:4,0x36:4,0x37:4,0x38:4,0x39:4,
0x3A:12,0x3B:11,0x3C:6,0x3D:6,0x3E:2,0x3F:2,0x40:2,
0x41:1,0x42:1,0x43:1,0x44:1,0x45:0,0x46:1,0x47:2,
0x48:5,0x49:5,0x4A:6,0x4B:0,0x4C:0,0x4D:0,0x4E:0,
}
TIMELINE = {2:0x81CF, 3:0x81A8, 4:0x7C12, 5:0x836F, 6:0x7F4B}

def u16(d,i):
    return struct.unpack_from('<H', d, i)[0]
def sx(v):
    return v-0x10000 if isinstance(v,int) and v>=0x8000 else v

def parse_sub(d,i):
    ops=[]
    while i < len(d):
        if d[i]==0xFF:
            return i+1, ops
        sid=d[i]
        if sid not in SKIP:
            return None, ops
        ops.append((sid, bytes(d[i+1:i+1+SKIP[sid]])))
        i += 1+SKIP[sid]
    return None, ops

def apply(fields, sid, raw):
    if sid != 0x02:
        return
    field, alu = raw[0], raw[1]
    imm = u16(raw, 2)
    cur = fields.get(field, 0)
    if isinstance(cur, tuple):
        if cur[0]=='rand' and alu==0x06:
            fields[field]=('add', cur[1], sx(imm)); return
        if cur[0]=='rand' and alu==0x1E:
            fields[field]=('shl', cur[1], imm & 15); return
        if cur[0]=='shl' and alu==0x06:
            fields[field]=('shl_add', cur[1], cur[2], sx(imm)); return
        fields[field]=('unk', alu, imm); return
    if alu==0x00: fields[field]=imm
    elif alu==0x06: fields[field]=(cur+imm)&0xFFFF
    elif alu==0x08: fields[field]=(cur-imm)&0xFFFF
    elif alu==0x12: fields[field]=cur & imm
    elif alu==0x14: fields[field]=cur | imm
    elif alu==0x16: fields[field]=cur ^ imm
    elif alu==0x1C: fields[field]=('rand', imm)
    elif alu==0x1E: fields[field]=(cur << (imm & 15)) & 0xFFFF
    else: fields[field]=('unk', alu, imm)

def child_overlay(d, ip, base=None):
    """Fallthrough init of a child script, applied on top of the parent fields.

    The child runs next frame on the entity the 0x22 sub-ops just wrote, so
    an OR or ADD sees that value. Returns fields, mover, counted.
    """
    counted = 0
    mover = 0
    fields = dict(base) if base else {}
    if ip < len(d)-7 and d[ip]==0x26 and d[ip+1]==0xAE:
        ip += 7
    # Global stores (the AE increment, then sometimes another word) sit
    # in front of the fallthrough sub-op block.
    skipped = 0
    while skipped < 4 and ip < len(d)-5 and d[ip]==0x2C:
        if d[ip+1]==0xAE and d[ip+2]==0x06:
            counted = 1
        ip += 5
        skipped += 1
    end = ip
    if ip < len(d) and d[ip]==0x20:
        end, ops = parse_sub(d, ip+1)
        if end:
            for sid, raw in ops:
                apply(fields, sid, raw)
        else:
            end = ip
    window = d[end:end+0x90]
    # A nested spawner fires children before its own mover. The script
    # is what drops the gate count; that script is not in the timeline.
    native_at = window.find(b'\x0c')
    spawn_at = -1
    for sop in (0x15, 0x16, 0x17, 0x18, 0x19):
        at = window.find(bytes([sop]))
        if at >= 0 and (spawn_at < 0 or at < spawn_at):
            spawn_at = at
    if spawn_at >= 0 and (native_at < 0 or spawn_at < native_at):
        counted = 0
    for native, mid in ((0x7066, 3),(0x7102, 4),(0x6F2E, 1),(0x6F7C, 2)):
        pat = bytes([0x0C, native & 0xFF, (native >> 8) & 0xFF])
        if pat in window:
            mover = mid
            break
    return fields, mover, counted

def take_spawn(d, ip):
    op = d[ip]
    if op not in (0x15,0x16,0x17,0x18,0x19):
        return None
    child = u16(d, ip+1)
    stype = u16(d, ip+3)
    sz = 7 if op in (0x18,0x19) else 5
    j = ip+sz
    fields = {0: stype}
    if j < len(d) and d[j] in (0x21,0x22,0x23,0x24):
        end, ops = parse_sub(d, j+3)
        if end:
            for sid, raw in ops:
                apply(fields, sid, raw)
            j = end
    fields, mover, counted = child_overlay(d, child, fields)
    if mover == 0 and isinstance(fields.get(0x28), int) and fields.get(0x28):
        mover = 1
    return j, child, fields, mover, counted

def norm_spawn(fields, mover, counted):
    def num(k, default=0):
        v = fields.get(k, default)
        if isinstance(v, tuple):
            return v
        return sx(v) if isinstance(v, int) else default
    x = fields.get(0x22, 0)
    x_mod = 0
    x_add = 0
    x_abs = 0
    if isinstance(x, tuple) and x[0]=='add':
        x_mod = x[1] & 0xFFFF
        x_add = x[2]
    elif isinstance(x, tuple) and x[0]=='rand':
        x_mod = x[1] & 0xFFFF
        x_add = 0
    elif isinstance(x, int):
        x_abs = sx(x)
    else:
        x_abs = 0
    def i16(k):
        v = fields.get(k, 0)
        if not isinstance(v, int):
            return 0
        return sx(v)
    def u16f(k):
        v = fields.get(k, 0)
        if not isinstance(v, int):
            return 0
        return v & 0xFFFF
    hp = i16(0x3C)
    if 0x3C not in fields:
        hp = 1
    return {
        'x': x_abs, 'x_mod': x_mod, 'x_add': x_add,
        'y': i16(0x24), 'hp': hp,
        'vx': u16f(0x26), 'vy': u16f(0x28),
        'ax': u16f(0x2A), 'ay': u16f(0x2C),
        'angle': u16f(0x2E), 'spin': u16f(0x30), 'speed': u16f(0x32),
        'bl': i16(0x42), 'br': i16(0x44), 'bt': i16(0x46), 'bb': i16(0x48),
        'type': u16f(0), 'mover': mover, 'counted': counted,
    }

def walk_timeline(d, ip, limit=20000):
    events=[]
    delay=0
    loops=[]
    calls=[]
    seen={}
    steps=0
    stopped='limit'
    while steps < limit:
        steps += 1
        key=(ip, delay, tuple((a,b) for a,b in loops))
        seen[key]=seen.get(key,0)+1
        if seen[key] > 4:
            stopped='loop'
            break
        if ip >= len(d):
            stopped='oob'; break
        op=d[ip]
        if op in (0x00,0x01):
            events.append(('end', delay)); break
        if op==0x04:
            delay += 1; ip += 1; continue
        if op==0x06:
            delay += u16(d, ip+1); ip += 3; continue
        if op==0x05:
            tgt=u16(d, ip+1)
            if tgt < 0x7000 or tgt >= len(d):
                stopped='jmp_out'; break
            ip = tgt; continue
        if op==0x08:
            # Near call. The timelines use this for helpers; entering them
            # walks child scripts. Skip the call site (operand is 2 bytes).
            ip += 3; continue
        if op==0x09:
            if not calls:
                stopped='ret'; break
            ip=calls.pop(); continue
        if op==0x0A:
            loops.append([ip+3, u16(d, ip+1)]); ip += 3; continue
        if op==0x0B:
            if loops and loops[-1][1] > 1:
                loops[-1][1] -= 1
                ip = loops[-1][0]
            else:
                if loops: loops.pop()
                ip += 1
            continue
        if op==0x26 and d[ip+1]==0xAE and d[ip+2] in (0x08,0x04):
            events.append(('gate', delay))
            delay = 0
            ip += 7
            continue
        if op==0x2B and d[ip+1]==0xC8 and d[ip+2]==0x00:
            events.append(('phase', delay, d[ip+3]))
            delay = 0
            ip += 4
            continue
        if op in (0x15,0x16,0x17,0x18,0x19):
            got = take_spawn(d, ip)
            if not got:
                stopped='badspawn'; break
            j, child, fields, mover, counted = got
            if child == 0x4848:
                ip = j
                continue
            events.append(('spawn', delay, norm_spawn(fields, mover, counted), child))
            delay = 0
            ip = j
            continue
        if op in (0x20,0x21,0x22,0x23,0x24,0x4F,0x50):
            j = ip+1 if op==0x20 else ip+3
            end,_ = parse_sub(d, j)
            if not end:
                stopped='badsub'; break
            ip = end
            continue
        if op==0x26:
            ip += 7; continue
        if op==0x25:
            ip += 6; continue
        if op in FIXED:
            ip += 1+FIXED[op]; continue
        stopped=f'stop {op:#x}@{ip:#x}'
        break
    return events, stopped, steps

def find_chain(d):
    """Return (list of phase rules, chain_ip)."""
    i = d.find(bytes.fromhex('25C800'), 0x7900, 0x7E00)
    if i < 0:
        return [], None
    arms=[]
    ip=i
    chain=None
    while ip < i+96 and d[ip]==0x25 and d[ip+1]==0xC8 and d[ip+2]==0x00:
        arms.append((d[ip+3], u16(d, ip+4)))
        ip += 6
    if ip < len(d) and d[ip]==0x05:
        chain=u16(d, ip+1)
    rules=[]
    for phase, tgt in arms:
        enter={'speed':None,'next':None}
        match={'op':0,'cell':0,'speed':None,'rewind':None,'next':None}
        p=tgt
        seen_cmp=False
        guard=0
        while guard<16 and p < len(d):
            guard += 1
            slot = match if seen_cmp else enter
            if d[p]==0x2C and d[p+1]==0x86 and d[p+2]==0x00:
                slot['speed']=u16(d, p+3); p+=5; continue
            if d[p]==0x2B and d[p+1]==0xC8 and d[p+2]==0x00:
                slot['next']=d[p+3]; p+=4; continue
            if d[p]==0x26 and d[p+1]==0x7C and d[p+2] in (0x02,0x04,0x06) and chain and u16(d, p+5)==chain:
                # Body is the fallthrough: the compare jumps to the wave chain.
                op=d[p+2]
                match['op'] = {0x02:1, 0x04:2, 0x06:3}[op]
                match['cell']=u16(d, p+3)
                seen_cmp=True
                p += 7
                continue
            if d[p]==0x2C and d[p+1]==0x7A and d[p+2]==0x12:
                p += 5
                if p < len(d) and d[p]==0x2C and d[p+1]==0x7A and d[p+2]==0x06:
                    match['rewind']=u16(d, p+3)
                    p += 5
                continue
            if d[p]==0x05:
                break
            break
        rules.append({
            'phase':phase,
            'enter_speed':enter['speed'],
            'enter_next':enter['next'],
            'match_op':match['op'],
            'match':match['cell'],
            'match_speed':match['speed'],
            'rewind':match['rewind'],
            'next':match['next'],
        })
    return rules, chain

def walk_chain(d, chain):
    rows=[]
    ip=chain
    guard=0
    seen=set()
    while ip and guard<800 and ip not in seen:
        seen.add(ip)
        guard += 1
        if d[ip]==0x05:
            tgt=u16(d, ip+1)
            if tgt in seen or tgt >= len(d):
                break
            if d[tgt]==0x26 and d[tgt+1]==0x7C and d[tgt+2]==0x00:
                ip=tgt
                continue
            break
        if not (d[ip]==0x26 and d[ip+1]==0x7C and d[ip+2]==0x00):
            break
        cell=u16(d, ip+3)
        tgt=u16(d, ip+5)
        p=tgt
        n=0
        while n<24 and p < len(d):
            n+=1
            if d[p] in (0x15,0x16,0x17,0x18,0x19):
                got=take_spawn(d, p)
                if not got:
                    break
                j, child, fields, mover, counted = got
                if child != 0x4848:
                    rows.append((cell, norm_spawn(fields, mover, counted), child))
                p=j
                continue
            if d[p]==0x05:
                break
            if d[p] in (0x20,0x21,0x22,0x23,0x24):
                end,_=parse_sub(d, p+3 if d[p]!=0x20 else p+1)
                if not end: break
                p=end
                continue
            break
        ip += 7
    return rows

def has_popup(d):
    return d.find(bytes.fromhex('15A3400090')) >= 0

def c_spawn(sp, idx_comment=''):
    def u(v):
        return f'{v & 0xFFFF}'
    def s(v):
        v = int(v)
        if v >= 0x8000:
            v -= 0x10000
        return str(v)
    return (
        '    { %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s },'
        % (s(sp['x']), u(sp['x_mod']), s(sp['x_add']), s(sp['y']), s(sp['hp']),
           u(sp['vx']), u(sp['vy']), u(sp['ax']), u(sp['ay']),
           u(sp['angle']), u(sp['spin']), u(sp['speed']),
           s(sp['bl']), s(sp['br']), s(sp['bt']), s(sp['bb']),
           u(sp['type']), sp['mover'], sp['counted'])
    )

def emit(path):
    chunks = {ci: Path(f'/tmp/chunks/c{ci}.bin').read_bytes() for ci in range(2, 7)}
    lines = []
    lines.append('/* Generated by tools/gen_stage_tables.py from decompressed SGM1 chunks 2-6.')
    lines.append(' * Timeline spawns, scroll-cell waves, and the phase switch. Do not edit by hand. */')
    lines.append('#include "stage_data.h"')
    lines.append('')
    infos = []
    for n, ci in enumerate(range(2, 7)):
        d = chunks[ci]
        ev, how, steps = walk_timeline(d, TIMELINE[ci])
        rules, chain = find_chain(d)
        waves = walk_chain(d, chain) if chain else []
        spawns = []
        index = {}
        def intern(sp):
            key = tuple(sorted(sp.items()))
            if key not in index:
                index[key] = len(spawns)
                spawns.append(sp)
            return index[key]
        events = []
        for e in ev:
            if e[0] == 'spawn':
                events.append((e[1], 0, intern(e[2])))
            elif e[0] == 'gate':
                events.append((e[1], 1, 0))
            elif e[0] == 'phase':
                events.append((e[1], 2, e[2]))
        wave_rows = []
        for cell, sp, child in waves:
            wave_rows.append((cell, intern(sp)))
        tag = f's{n+1}'
        lines.append(f'static const RbSpawnDef {tag}_spawns[] = {{')
        if not spawns:
            lines.append('    { 0 }')
        for sp in spawns:
            lines.append(c_spawn(sp))
        lines.append('};')
        lines.append(f'static const RbStageEvent {tag}_events[] = {{')
        if not events:
            lines.append('    { 0, 0, 0 }')
        for delay, kind, arg in events:
            lines.append(f'    {{ {delay}, {kind}, {arg} }},')
        lines.append('};')
        lines.append(f'static const RbCellWave {tag}_waves[] = {{')
        if not wave_rows:
            lines.append('    { 0, 0 }')
        for cell, si in wave_rows:
            lines.append(f'    {{ {cell}, {si} }},')
        lines.append('};')
        lines.append(f'static const RbPhaseRule {tag}_rules[] = {{')
        if not rules:
            lines.append('    { 0, 0, 0, 0xFFFF, 0xFFFF, 0, 0xFF, 0xFF }')
        for r in rules:
            es = 0xFFFF if r['enter_speed'] is None else r['enter_speed']
            ms = 0xFFFF if r['match_speed'] is None else r['match_speed']
            en = 0xFF if r['enter_next'] is None else r['enter_next']
            nx = 0xFF if r['next'] is None else r['next']
            rw = 0 if r['rewind'] is None else r['rewind']
            lines.append(
                '    { %d, %d, %d, %d, %d, %d, %d, %d },'
                % (r['phase'], r['match_op'], r['match'], es, ms, rw, en, nx)
            )
        lines.append('};')
        lines.append('')
        infos.append((tag, len(spawns), len(events), len(wave_rows), len(rules), 1 if has_popup(d) else 0, how, len([e for e in ev if e[0]=='spawn'])))
    lines.append('const RbStageInfo rb_stages[RB_STAGE_COUNT] = {')
    for tag, ns, ne, nw, nr, popup, how, nspawn in infos:
        sc = ns if ns else 1
        ec = ne if ne else 1
        wc = nw if nw else 1
        rc = nr if nr else 1
        # empty arrays still have 1 dummy element; counts of 0 would walk it.
        # Use the real counts. Dummy element is only present when count is 0,
        # and the runtime checks count before indexing.
        lines.append(
            '    { %s_spawns, %s_events, %s_waves, %s_rules, %d, %d, %d, %d, %d },'
            % (tag, tag, tag, tag, ns, ne, nw, nr, popup)
        )
    lines.append('};')
    lines.append('')
    Path(path).write_text('\n'.join(lines) + '\n')
    for row in infos:
        print('emitted', row)

def main():
    chunks={}
    for ci in range(2,7):
        chunks[ci]=Path(f'/tmp/chunks/c{ci}.bin').read_bytes()
    for ci, d in chunks.items():
        ev, how, steps = walk_timeline(d, TIMELINE[ci])
        rules, chain = find_chain(d)
        waves = walk_chain(d, chain) if chain else []
        sp = [e for e in ev if e[0]=='spawn']
        print(f'c{ci} tl {how} steps {steps} events {len(ev)} spawns {len(sp)} waves {len(waves)} popup {has_popup(d)} chain {hex(chain) if chain else None}')
        for r in rules:
            print('  rule', r)
        # mover mix
        from collections import Counter
        print('  movers', Counter(e[2]['mover'] for e in sp))
        print('  counted', sum(1 for e in sp if e[2]['counted']))
        if waves[:2]:
            print('  wave0', waves[0][0], waves[0][1]['x'], waves[0][1]['hp'], 'mv', waves[0][1]['mover'])
    emit('/workspace/src/core/stage_data.c')

if __name__=='__main__':
    main()

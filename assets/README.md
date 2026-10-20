# Assets (not committed)

Copyrighted PC-98 dumps stay outside git. Extract locally:

```bash
python3 tools/extract_pc98_hdm.py /path/to/Rude\ Breaker.hdm assets/raw
python3 tools/parse_sgm1.py assets/raw/RUDEBR/SGM1.DAT assets/sgm1
```

Then run:

```bash
RUDEBR_ASSETS=assets/raw/RUDEBR ./build/rudebreaker
```

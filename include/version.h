#ifndef RB_VERSION_H
#define RB_VERSION_H

/* Port version. Shown by --version, in the window title and on the
 * CONFIG page; the manuals carry the same number. A build can pass
 * -DRB_VERSION="..." to override it. */
#ifndef RB_VERSION
#define RB_VERSION "0.10.0"
#endif

/* Version as the CONFIG page prints it (the host font draws the
 * full stop). */
#ifndef RB_VERSION_MENU
#define RB_VERSION_MENU RB_VERSION
#endif

#endif

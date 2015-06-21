
#include <config.h>
#include <errno.h>

#include "lib/global.h"
#include "lib/tty/tty.h"
#include "lib/tty/mouse.h"
#include "lib/vfs/vfs.h"
#include "lib/strutil.h"
#include "lib/util.h"           /* load_file_position() */
#include "lib/widget.h"

#include "src/filemanager/layout.h"     /* menubar_visible */
#include "src/filemanager/midnight.h"   /* the_menubar */

#include "internal.h"
#include "imgviewer.h"

#define W3M_PATH "/usr/lib/w3m/w3mimgdisplay"
#define PW (6)
#define PH (12)

gboolean
is_image_file(const gchar *file)
{
    int slen;
    const gchar *last4;

    slen = strlen(file);
    last4 = file + slen - 4;
    if(g_ascii_strcasecmp(last4, ".jpg") == 0 ||
       g_ascii_strcasecmp(last4, ".png") == 0 ||
       g_ascii_strcasecmp(last4, ".gif") == 0) {
        // printf("IMAGE FILE!\n");
        return TRUE;
    }

    return FALSE; 
}

gboolean
draw_image_file(const char *file, int tx, int ty, int tw, int th)
{
    int dx, dy, dw, dh;
    int w, h, parsed;
    FILE* sz_file;
    FILE *open_file;
    char cmd[2048];

    tx *= PW;
    tw *= PW;
    ty *= PH;
    th *= PH;

    memset(cmd, 0, 2048);
    snprintf(cmd, 2047, "echo \"5;%s\" | %s", file, W3M_PATH);
    // printf("cmd: %s", cmd);
    sz_file = popen(cmd, "r");
    parsed = fscanf(sz_file, "%d %d", &w, &h);
 
    fclose(sz_file);

    if(parsed != 2 || w <= 0 || h <= 0) {
        return TRUE;
    }
    // printf("image size is %dx%d\n", w, h);

    dw = tw;
    dh = th;

    if(tw * h > th * w) {
        dw = th * w / h; 
    }
    else {
        dh = tw * h / w;
    }

    dx = tx + ((tw - dw) / 2);
    dy = ty + ((th - dh) / 2);

    // printf("Img %dx%d fits in %dx%d as %dx%d at %dx%d\n",
    //     w, h, tw, th, dw, dh, dx, dy);

    open_file = popen(W3M_PATH, "w");
    fprintf(open_file, "0;1;%d;%d;%d;%d;;;;;%s\n4;\n3;\n",
        dx, dy, dw, dh, file);
    memset(cmd, 0, 2048);
    fclose(open_file);

}

gboolean
draw_image_fd(int fd, int tx, int ty, int tw, int th)
{
    int tmp_file;
    int rd;
    char buffer[4096];

    const char *tmp_file_name = "/tmp/tempy";

    // printf("draw_image_file %d %d %d %d\n", tx, ty, tw ,th);
    //getchar();

    tmp_file = open(tmp_file_name, O_WRONLY | O_BINARY);
    do {
        rd = mc_read(fd, buffer, 4096);
        write(tmp_file, buffer, rd);
        // printf("read %d\n", rd);
    } while(rd > 0);

    close(tmp_file);

    draw_image_file(tmp_file_name, tx, ty, tw, th);

    return TRUE;
}

gboolean
clear_area(int tx, int ty, int tw, int th)
{
    FILE* clear_file;

    tx *= PW;
    tw *= PW + 4;
    ty *= PH;
    th *= PH + 4;

    clear_file = popen(W3M_PATH, "w");
    fprintf(clear_file, "6;%d;%d;%d;%d\n4;\n3;\n", tx, ty, tw, th);
    fclose(clear_file);

    return TRUE;
}


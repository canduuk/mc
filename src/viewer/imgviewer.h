

gboolean
is_image_file(const gchar *file);

gboolean
draw_image_fd(int fd, int tx, int ty, int tw, int th);

gboolean
draw_image_file(const char *file, int tx, int ty, int tw, int th);

gboolean
clear_area(int tx, int ty, int tw, int th);


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gst/gst.h>

typedef struct {
    char *frame_count;
    char *fps_rate;
    char *output_dir;
} config_t;

const char *plugin_path_var = "GST_PLUGIN_PATH";

int check_config(const config_t* cfg) {
    // Add proper config check
    // For example frame count is not negative, fps is a valid number, etc.
    if (cfg->frame_count == NULL) {
        printf("Please specify [-n frame_count]\n");
        return -1;
    }
    if (cfg->fps_rate == NULL) {
        printf("Please specify [-f fps_rate]\n");
        return -1;
    }
    if (cfg->output_dir == NULL) {
        printf("Please specify [-o output_dir]\n");
        return -1;
    }
    return 0;
}

char *build_command(const config_t* cfg) {
    char *command;
    int cmd_size = 0;
    
    // Don't forget - strlen doesn't count NULL character
    cmd_size = strlen("pylonsrc testimage=1 num-buffers= fps= ! timeoverlay halignment=right valignment=bottom ! clockoverlay halignment=left valignment=bottom time-format=%Y/%m/%d %H:%M:%S ! pngenc ! multifilesink location=%d.png") + strlen(cfg->output_dir) + strlen(cfg->fps_rate) + strlen(cfg->frame_count) + 4;
    
    command = malloc(cmd_size);
    
    if (command != NULL) {
        snprintf(command, cmd_size, "pylonsrc testimage=1 num-buffers=%s fps=%s ! timeoverlay halignment=right valignment=bottom ! clockoverlay halignment=left valignment=bottom time-format=\"%%Y/%%m/%%d %%H:%%M:%%S\" ! pngenc ! multifilesink location=%s%%d.png",
        cfg->frame_count,
        cfg->fps_rate,
        cfg->output_dir);
    }
    
    return command;
}

int main(int argc, char *argv[]) {
    config_t app_config = {NULL, NULL, NULL};
    int opt;
    int status;

    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;

    while((opt = getopt(argc, argv, "n:f:o:")) != -1) {
        switch(opt) {
        case 'n':
            app_config.frame_count = optarg;
            break;
        case 'f':
            app_config.fps_rate = optarg;
            break;
        case 'o':
            app_config.output_dir = optarg;
            break;
        default:
            printf("Usage: %s [-n frame_count] [-f fps_rate] [-o output_dir]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    
    status = check_config(&app_config);
    if (status != 0) {
        printf("Usage: %s [-n frame_count] [-f fps_rate] [-o output_dir]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    if (!getenv(plugin_path_var)) {
        printf("Please specify GST_PLUGIN_PATH variable for pylon plugin\n");
        exit(EXIT_FAILURE);
    }
    
    char *command = build_command(&app_config);
    
    if (command == NULL) {
        exit(EXIT_FAILURE);
    }
    
    gst_init(&argc, &argv);
    pipeline = gst_parse_launch(command, NULL);
    
    // TODO: Add appsink with a callback for a new-sample signal. On receiving "new-sample" signal
    // appsink would retreive GST_BUFFER_TIMESTAMP and use it as for a filename.
    // Replace multifilesink with appsink name="testsink"
    // ProgramData *data = NULL;
    // GsElement *testsink = NULL;
    // testsink = gst_bin_get_by_name(GST_BIN(pipeline), "testsink");
    // g_object_set (G_OBJECT (testsink), "emit-signals", TRUE, "sink", FALSE, NULL);
    // g_signal_connect (testsink, "new-sample", G_CALLBACK(on_new_sample), data);
    // Add actual callback
    
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
        g_error("Error!\n");
    }
    
    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    free(command);

    exit(EXIT_SUCCESS);
}

1) Download and install pylon sdk from Basler website
2) Clone git from gst-plugins-vision.git
3) Modify cmake/modulesFindPylon.cmake
    a. /opt/pylon5 to the actual installation folder.
4) Comment out Static_assert in gstpylonsrc.c
    G_STATIC_ASSERT ((int) PROP_NUM_PROPERTIES == GST_PYLONSRC_NUM_PROPS);
5) Follow gst-plugins-vision build instructions
6) Export PYLON_CAMEMU=1
7) Export GST_PLUGIN_PATH with a path to gst-plugins-vision folder (build/sys)
8) Build a sample application
    make
9) Run it
    /.save-samples -n 10 -f 5 -o ./

Limitations:
I couldn't fine a way to modify a filename with date and time for multifilesink for every file created.
Location property is set once and can't be modified in runtime.
So I added the time and date as an overlay to an image itself.
The proper soluton would be to use appsink. On receiving a new frame appsink would save it to a file with a correct filename.

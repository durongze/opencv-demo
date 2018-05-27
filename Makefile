 EXEC = exec
 OBJS = test.o
 
 CC = gcc
 CXX = g++
 
 CFLAGS =
 
 CXXFLAGS =
 
 VPATH = src
 
 INCPATH = -I${FREEGLUT300_HOME}/include
 LIBPATH = -L${$FREEGLUT300_HOME}/lib
 
 LIBS = -lpthread
 LIBS += -lopencv_calib3d
 LIBS += -lopencv_core
 LIBS += -lopencv_dnn
 LIBS += -lopencv_features2d
 LIBS += -lopencv_flann
 LIBS += -lopencv_highgui
 LIBS += -lopencv_imgcodecs
 LIBS += -lopencv_imgproc
 LIBS += -lopencv_ml
 LIBS += -lopencv_objdetect
 LIBS += -lopencv_photo
 LIBS += -lopencv_shape
 LIBS += -lopencv_stitching
 LIBS += -lopencv_superres
 LIBS += -lopencv_videoio
 LIBS += -lopencv_video
 LIBS += -lopencv_videostab
 
 $(EXEC):$(OBJS)
     $(CXX) $(INCPATH) $(LIBPATH) -o $(EXEC)  $(OBJS) $(LIBS)
 
 %.o:%.cpp
     $(CXX) $(INCPATH) $(LIBPATH) -o $@ -c $< $(LIBS) 
 
 clean:
     $(RM) $(OBJS)

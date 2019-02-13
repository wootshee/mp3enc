SUBDIRS = extern/lame

AM_CXXFLAGS = -I$(top_srcdir)/src/extern/lame/include @AM_CXXFLAGS@

bin_PROGRAMS = mp3enc
mp3enc_SOURCES = main.cpp encoder-pool.cpp glob-posix.cpp mp3encoder.cpp platform-posix.cpp wavfile.cpp
mp3enc_LDADD = extern/lame/libmp3lame/.libs/libmp3lame.a

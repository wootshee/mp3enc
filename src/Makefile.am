SUBDIRS = extern/lame

AM_CXXFLAGS = -I$(topsrcdir) -I$(srcdir)/extern/lame/include @AM_CXXFLAGS@

bin_PROGRAMS = mp3enc
mp3enc_SOURCES = mp3enc.cpp
mp3enc_LDADD = extern/lame/libmp3lame/.libs/libmp3lame.a

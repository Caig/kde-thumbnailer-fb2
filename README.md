kde-thumbnailer-fb2
===================

A KDE thumbnail generator for the FictionBook file format.

Currently it doesn't support *.fb2.zip files.

Installation from source
------------------------

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ..
    make
    sudo make install
    kbuildsycoca4
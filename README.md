kde-thumbnailer-fb2
===================

A KDE thumbnail generator for the FictionBook file format.

http://kde-apps.org/content/show.php/KDE+fb2+Thumbnailer?content=160180

Installation from source
------------------------

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ..
    make
    sudo make install
    kbuildsycoca4

Installation
============

Installation environment  we used : ``Ubuntu 22.04 LTS`` , ``WSL2``

The BerkeleyDB version we used : ``db-18.1.32``

Download Berkeley DB
--------------------

`libdb`_ or `db-18.1.32.tar.gz`_

If you downloaded the ``db-18.1.32.tar.gz`` file, unzip it.

.. code-block:: bash

    tar xvfz db-18.1.32.tar.gz


Go to ``build_unix`` directory.

.. code-block:: bash

    cd db-18.1.32
    cd build_unix

.. code-block:: bash

    ../dist/configure \
    --enable-dbm \
    --enable-sql \
    --enable-o_direct \
    --enable-localization \
    --enable-umrw \
    --enable-cxx \
    --enable-atomicfileread \
    --enable-sql_compat \
    CFLAGS="-I/usr/local/ssl/include -L/usr/local/ssl/lib -DSQLITE_DIRECT_OVERFLOW_READ -DSQLITE_ENABLE_DBSTAT_VTAB -DSQLITE_ENABLE_DBPAGE_VTAB -DSQLITE_ENABLE_RTREE -DSQLITE_USE_ALLOCA -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1 -DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_ENABLE_RBU"

.. code-block:: bash

    make
    sudo make install


Edit etc/profile
----------------

.. code-block:: bash

    LD_LIBRARY_PATH=/usr/local/BerkeleyDB.18.1/lib:$LD_LIBRARY_PATH
    export LD_LIBRARY_PATH


Error "db.h: No such file or directory"
---------------------------------------

.. code-block:: bash

    sudo apt install libdb-dev
    sudo apt-get install libdb4o-cil-dev


If you run the above code and still get an error

.. code-block:: bash

    sudo apt install libdb4-dev
    sudo apt-get install libc6-i386 lib32gcc1
    sudo apt-get install ia32-libs g++-multilib



.. _libdb:
    https://github.com/berkeleydb/libdb

.. _db-18.1.32.tar.gz:
    https://github.com/Mindlestick/TinyIoT/raw/main/db-18.1.32.tar.gz
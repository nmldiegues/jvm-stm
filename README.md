Providing Transactional Semantics
Instructions to install the modified Virtual Machine

The project should contain the following folders:
 * jamvm
 * tinySTM
 * classpath
 * tests

Inside classpath directory:
 * ./configure --without-x --disable-gtk-peer --disable-gconf-peer --disable-plugin --disable-tools --disable-examples
 * make clean
 * make
 * make install

There may be an error while compilling. Find the Makefile in that folder, search for AM_CFLAGS and add -fno-strict-aliasing


Inside the tinySTM directory:
 * Verify that the following configurations in the Makefile are as described:
 * DEFINES += -DDESIGN=WRITE_BACK_ETL	(other two are disabled)
 * DEFINES += -DCM=CM_DELAY	(other three are disabled)
 * DEFINES += -UIRREVOCABLE_ENABLED
 * DEFINES += -UNO_DUPLICATES_IN_RW_SETS
 * DEFINES += -DWAIT_YIELD
 * DEFINES += -UUSE_BLOOM_FILTER
 * DEFINES += -UEPOCH_GC
 * DEFINES += -UCONFLICT_TRACKING
 * DEFINES += -UREAD_LOCKED_DATA
 * DEFINES += -ULOCK_IDX_SWAP
 * All debugs are off
 * DEFINES += -DRW_SET_SIZE=4096000
 * CFLAGS += -fPIC
 * make clean
 * make

Inside the jamvm directory:
 * Inside lib folder: ln -s ../../tinySTM-1.0.0/lib/libstm.a libstm.a
 * Inside src folder: ln -s ../../tinySTM-1.0.0/include/stm.h stm.h
 * Inside src folder: ln -s ../../tinySTM-1.0.0/include/wrappers.h wrappers.h
 * ./configure
 * Modify the generated Makefile in src/: LIBS = -lz -ldl -lm -lpthread ../lib/libstm.a
 * Modify the generated Makefile in src/interp/: LIBS = -lz -ldl -lm -lpthread ../../lib/libstm.a
 * Modify the generated Makefile in src/interp/engine/: LIBS = -lz -ldl -lm -lpthread ../../../lib/libstm.a
 * make clean
 * make
 * make install

Inside the tests directory:
 * For each package tN in packages t1-t7, compile with javac tN/TestAll.java
 * For package t8, compile with javac t8/TestCounterTiny.java & javac -cp t8/build/jvstm.jar:. t8/TestCounterJVSTM.java
 * For package t9, compile with javac t9/TestTransfersTiny.java & javac -cp t8/build/jvstm.jar:. t9/TestTransfersJVSTM.java
 * For each package tN in packages t11-t13, compile with javac tN/Main.java
 * Run each of the compiled tests with the jamvm executable and the parameters required by the program.
 * Run again with a pristine version of the JamVM to compare the results, or remove the @Atomic annotations.

cd /home/runner
export PATH=/usr/bin:/bin:/tool/pandora64/bin
export LIBRARY_PATH=:/playground_lib/systemc-2.3.3/lib-linux64
export LD_LIBRARY_PATH=/playground_lib/systemc-2.3.3/lib-linux64
export CPATH=:/playground_lib/systemc-2.3.3/include
export HOME=/home/runner
g++ -o sim -lsystemc *.cpp  && echo "Compile done. Starting run..." && ./sim  ; echo 'Creating result.zip...' && zip -r /tmp/tmp_zip_file_123play.zip . && mv /tmp/tmp_zip_file_123play.zip result.zip
CURRENT_DIR=$PWD
PARENT_DIR="$(dirname "$PWD")"
TMP_DIR=$PARENT_DIR/tmp
BUILD_DIR=$TMP_DIR/gyro-sms-release
SOURCE_DIR=$PARENT_DIR/gyro-sms
OUT_DIR=$PARENT_DIR/out-release
if [ -f ./gyro ]; then
  echo "link gyro exist!"
else
  cd $SOURCE_DIR && ln -s $PARENT_DIR/gyro gyro
fi
mkdir -p $TMP_DIR && mkdir -p $BUILD_DIR \
 && cd $BUILD_DIR \
 && cmake $SOURCE_DIR \
 -DCMAKE_BUILD_TYPE=Release \
 -DBOOST_ROOT=$OUT_DIR/include \
 -DBOOST_INCLUDEDIR=$OUT_DIR/include \
 -DBOOST_LIBRARYDIR=$OUT_DIR/lib/ \
 -DBUILD_GUI_DEPS=ON \
 -DMANUAL_SUBMODULES=1 \
 -DBUILD_TESTS=OFF \
 -DCMAKE_INSTALL_PREFIX=$OUT_DIR \
 && make -j2 && make install

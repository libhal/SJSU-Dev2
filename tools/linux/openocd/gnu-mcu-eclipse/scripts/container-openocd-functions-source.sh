# -----------------------------------------------------------------------------

# Helper script used in the second edition of the GNU MCU Eclipse build 
# scripts. As the name implies, it should contain only functions and 
# should be included with 'source' by the container build scripts.

# -----------------------------------------------------------------------------

function download_openocd() 
{
  if [ ! -d "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}" ]
  then
    (
      xbb_activate

      cd "${WORK_FOLDER_PATH}"
      git_clone "${OPENOCD_GIT_URL}" "${OPENOCD_GIT_BRANCH}" \
          "${OPENOCD_GIT_COMMIT}" "${OPENOCD_SRC_FOLDER_NAME}"
      cd "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}"
      git submodule update --init --recursive --remote
    )
  fi
}

# -----------------------------------------------------------------------------

function do_openocd()
{

  local openocd_stamp_file_path="${INSTALL_FOLDER_PATH}"/stamp-openocd-installed

  if [ ! -f "${openocd_stamp_file_path}" ]
  then

    download_openocd

    (
      xbb_activate

      cd "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}"
      if [ ! -d "autom4te.cache" ]
      then
        ./bootstrap
      fi

      mkdir -p "${BUILD_FOLDER_PATH}/${OPENOCD_FOLDER_NAME}"
      cd "${BUILD_FOLDER_PATH}/${OPENOCD_FOLDER_NAME}"

      export JAYLINK_CFLAGS='${EXTRA_CFLAGS} -fvisibility=hidden'

      if [ "${TARGET_OS}" == "win" ]
      then

        # --enable-minidriver-dummy -> configure error
        # --enable-zy1000 -> netinet/tcp.h: No such file or directory

        # --enable-openjtag_ftdi -> --enable-openjtag
        # --enable-presto_libftdi -> --enable-presto
        # --enable-usb_blaster_libftdi -> --enable-usb_blaster

        export OUTPUT_DIR="${BUILD_FOLDER_PATH}"
        
        export CFLAGS="${EXTRA_CXXFLAGS} -Wno-pointer-to-int-cast" 
        export CXXFLAGS="${EXTRA_CXXFLAGS}" 
        export LDFLAGS="${EXTRA_LDFLAGS} -static"

        AMTJTAGACCEL="--enable-amtjtagaccel"
        # --enable-buspirate -> not supported on mingw
        BUSPIRATE="--disable-buspirate"
        GW18012="--enable-gw16012"
        PARPORT="--enable-parport"
        PARPORT_GIVEIO="--enable-parport-giveio"
        # --enable-sysfsgpio -> available only on Linux
        SYSFSGPIO="--disable-sysfsgpio"

      elif [ "${TARGET_OS}" == "linux" ]
      then

        # --enable-minidriver-dummy -> configure error

        # --enable-openjtag_ftdi -> --enable-openjtag
        # --enable-presto_libftdi -> --enable-presto
        # --enable-usb_blaster_libftdi -> --enable-usb_blaster

        export CFLAGS="${EXTRA_CFLAGS} -Wno-format-truncation -Wno-format-overflow"
        export CXXFLAGS="${EXTRA_CXXFLAGS}"
        export LDFLAGS="${EXTRA_LDFLAGS}" 
        export LIBS="-lpthread -lrt -ludev"

        AMTJTAGACCEL="--enable-amtjtagaccel"
        BUSPIRATE="--enable-buspirate"
        GW18012="--enable-gw16012"
        PARPORT="--enable-parport"
        PARPORT_GIVEIO="--enable-parport-giveio"
        SYSFSGPIO="--enable-sysfsgpio"

      elif [ "${TARGET_OS}" == "macos" ]
      then

        # --enable-minidriver-dummy -> configure error

        # --enable-openjtag_ftdi -> --enable-openjtag
        # --enable-presto_libftdi -> --enable-presto
        # --enable-usb_blaster_libftdi -> --enable-usb_blaster

        export CFLAGS="${EXTRA_CFLAGS}"
        export CXXFLAGS="${EXTRA_CXXFLAGS}"
        export LDFLAGS="${EXTRA_LDFLAGS}"
        export LIBS="-lobjc"

        # --enable-amtjtagaccel -> 'sys/io.h' file not found
        AMTJTAGACCEL="--disable-amtjtagaccel"
        BUSPIRATE="--enable-buspirate"
        # --enable-gw16012 -> 'sys/io.h' file not found
        GW18012="--disable-gw16012"
        PARPORT="--disable-parport"
        PARPORT_GIVEIO="--disable-parport-giveio"
        # --enable-sysfsgpio -> available only on Linux
        SYSFSGPIO="--disable-sysfsgpio"

      else

        echo "Unsupported target os ${TARGET_OS}."
        exit 1

      fi

      if [ ! -f "config.status" ]
      then

        # May be required for repetitive builds, because this is an executable built 
        # in place and using one for a different architecture may not be a good idea.
        rm -rfv "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}"/jimtcl/autosetup/jimsh0

        echo
        echo "Running openocd configure..."
      
        (
          bash "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}"/configure --help

          bash "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}"/configure \
            --prefix="${INSTALL_FOLDER_PATH}/${APP_LC_NAME}"  \
            \
            --build=${BUILD} \
            --host=${HOST} \
            --target=${TARGET} \
            \
            --datarootdir="${INSTALL_FOLDER_PATH}" \
            --localedir="${APP_PREFIX}"/share/locale  \
            --mandir="${APP_PREFIX_DOC}"/man  \
            --pdfdir="${APP_PREFIX_DOC}"/pdf  \
            --infodir="${APP_PREFIX_DOC}"/info \
            --docdir="${APP_PREFIX_DOC}"  \
            \
            --disable-wextra \
            --disable-werror \
            --enable-dependency-tracking \
            \
            --enable-branding="GNU MCU Eclipse" \
            \
            --enable-aice \
            ${AMTJTAGACCEL} \
            --enable-armjtagew \
            --enable-at91rm9200 \
            --enable-bcm2835gpio \
            ${BUSPIRATE} \
            --enable-cmsis-dap \
            --enable-dummy \
            --enable-ep93xx \
            --enable-ftdi \
            ${GW18012} \
            --disable-ioutil \
            --enable-jlink \
            --enable-jtag_vpi \
            --disable-minidriver-dummy \
            --disable-oocd_trace \
            --enable-opendous \
            --enable-openjtag \
            --enable-osbdm \
            ${PARPORT} \
            --disable-parport-ppdev \
            ${PARPORT_GIVEIO} \
            --enable-presto \
            --enable-remote-bitbang \
            --enable-riscv \
            --enable-rlink \
            --enable-stlink \
            ${SYSFSGPIO} \
            --enable-ti-icdi \
            --enable-ulink \
            --enable-usb-blaster \
            --enable-usb_blaster_2 \
            --enable-usbprog \
            --enable-vsllink \
            --disable-zy1000-master \
            --disable-zy1000 \

        ) 2>&1 | tee "${INSTALL_FOLDER_PATH}"/configure-openocd-output.txt
        cp "config.log" "${INSTALL_FOLDER_PATH}"/config-openocd-log.txt

      fi

      echo
      echo "Running openocd make..."
      
      (
        make ${JOBS} bindir="bin" pkgdatadir=""
        if [ "${WITH_STRIP}" == "y" ]
        then
          make install-strip
        else
          make install  
        fi

        if [ "${WITH_PDF}" == "y" ]
        then
          make ${JOBS} bindir="bin" pkgdatadir="" pdf 
          make install-pdf
        fi

        if [ "${WITH_HTML}" == "y" ]
        then
          make ${JOBS} bindir="bin" pkgdatadir="" html
          make install-html
        fi

        if [ "${TARGET_OS}" == "linux" ]
        then
          # Workaround to Docker error on 32-bit image:
          # stat: Value too large for defined data type
          rm -rf /tmp/openocd
          cp "${APP_PREFIX}"/bin/openocd /tmp/openocd
          patchelf --set-rpath '$ORIGIN' /tmp/openocd
          cp /tmp/openocd "${APP_PREFIX}"/bin/openocd 

          copy_linux_system_so libudev
        elif [ "${TARGET_OS}" == "macos" ]
        then
          change_dylib "libgcc_s.1.dylib" "${APP_PREFIX}"/bin/openocd
        elif [ "${TARGET_OS}" == "win" ]
        then
          # For unknown reasons, openocd still has a reference to libusb0.dll,
          # although everything should have been compiled as static.
          cp -v "${INSTALL_FOLDER_PATH}"/bin/libusb0.dll \
            "${APP_PREFIX}"/bin
        fi

      ) 2>&1 | tee "${INSTALL_FOLDER_PATH}"/make-openocd-output.txt
    )

    if [ "${TARGET_OS}" != "win" ]
    then
      echo
      "${APP_PREFIX}"/bin/openocd --version
    fi

    touch "${openocd_stamp_file_path}"
  else
    echo "Component openocd already installed."
  fi
}

# -----------------------------------------------------------------------------

function copy_gme_files()
{
  rm -rf "${APP_PREFIX}/${DISTRO_LC_NAME}"
  mkdir -p "${APP_PREFIX}/${DISTRO_LC_NAME}"

  echo
  echo "Copying license files..."

  copy_license \
    "${WORK_FOLDER_PATH}/${LIBUSB1_SRC_FOLDER_NAME}" \
    "${LIBUSB1_FOLDER_NAME}"

  if [ "${TARGET_OS}" != "win" ]
  then
    copy_license \
      "${WORK_FOLDER_PATH}/${LIBUSB0_SRC_FOLDER_NAME}" \
      "${LIBUSB0_FOLDER_NAME}"
  else
    copy_license \
      "${WORK_FOLDER_PATH}/${LIBUSB_W32_SRC_FOLDER_NAME}" \
      "${LIBUSB_W32_FOLDER_NAME}"
  fi

  copy_license \
    "${WORK_FOLDER_PATH}/${LIBFTDI_SRC_FOLDER_NAME}" \
    "${LIBFTDI_FOLDER_NAME}"
  copy_license \
    "${WORK_FOLDER_PATH}/${LIBICONV_SRC_FOLDER_NAME}" \
    "${LIBICONV_FOLDER_NAME}"

  copy_license \
    "${WORK_FOLDER_PATH}/${OPENOCD_SRC_FOLDER_NAME}" \
    "${OPENOCD_FOLDER_NAME}"

  copy_build_files

  echo
  echo "Copying GME files..."

  cd "${WORK_FOLDER_PATH}"/build.git
  /usr/bin/install -v -c -m 644 "README-${RELEASE_VERSION}.md" \
    "${APP_PREFIX}"/README.md
}

function check_binaries()
{
  if [ "${TARGET_OS}" == "linux" ]
  then

    echo
    echo "Checking binaries for unwanted shared libraries..."

    check_binary "${APP_PREFIX}"/bin/openocd

    local binaries=$(find "${APP_PREFIX}"/bin -name \*.so.\* -type f)
    for bin in ${binaries} 
    do
      check_library ${bin}
    done

  elif [ "${TARGET_OS}" == "macos" ]
  then

    echo
    echo "Checking binaries for unwanted dynamic libraries..."

    check_binary "${APP_PREFIX}"/bin/openocd

    local binaries=$(find "${APP_PREFIX}"/bin -name \*.dylib -type f)
    for bin in ${binaries} 
    do
      check_library ${bin}
    done

  elif [ "${TARGET_OS}" == "win" ]
  then

    echo
    echo "Checking binaries for unwanted DLLs..."

    check_binary "${APP_PREFIX}"/bin/openocd.exe

  else

    echo "Unsupported TARGET_OS ${TARGET_OS}"
    exit 1

  fi
}


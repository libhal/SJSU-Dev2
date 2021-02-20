# -----------------------------------------------------------------------------

# Helper script used in the second edition of the GNU MCU Eclipse build 
# scripts. As the name implies, it should contain only functions and 
# should be included with 'source' by the host build scripts.

# -----------------------------------------------------------------------------

function host_get_current_date() 
{
  # Use the UTC date as version in the name of the distribution file.
  DISTRIBUTION_FILE_DATE=${DISTRIBUTION_FILE_DATE:-$(date -u +%Y%m%d-%H%M)}

  # Leave a track of the start date, in case of resume needed.
  touch "${HOST_WORK_FOLDER_PATH}/${DISTRIBUTION_FILE_DATE}"
  echo
  echo "DISTRIBUTION_FILE_DATE=\"${DISTRIBUTION_FILE_DATE}\""
}

function host_start_timer() 
{
  HOST_BEGIN_SECOND=$(date +%s)
  echo
  echo "Script \"$0\" started at $(date)."
}

function host_stop_timer() 
{
  local host_end_second=$(date +%s)
  echo
  echo "Script \"$0\" completed at $(date)."
  local delta_seconds=$((host_end_second-HOST_BEGIN_SECOND))
  if [ ${delta_seconds} -lt 100 ]
  then
    echo "Duration: ${delta_seconds} seconds."
  else
    local delta_minutes=$(((delta_seconds+30)/60))
    echo "Duration: ${delta_minutes} minutes."
  fi

  if [ "${HOST_UNAME}" == "Darwin" ]
  then
    say "Wake up, the build completed successfully"
  fi
}

# -----------------------------------------------------------------------------

function host_detect() 
{
  echo
  uname -a

  HOST_DISTRO_NAME=""
  HOST_UNAME="$(uname)"
  HOST_MACHINE="$(uname -m)"

  if [ "${HOST_UNAME}" == "Darwin" ]
  then
    # uname -p -> i386
    # uname -m -> x86_64

    HOST_BITS="64"

    HOST_DISTRO_NAME=Darwin
    HOST_DISTRO_LC_NAME=darwin

  elif [ "${HOST_UNAME}" == "Linux" ]
  then
    # ----- Determine distribution name and word size -----

    # uname -p -> x86_64/i686
    # uname -m -> x86_64/i686

    set +e
    HOST_DISTRO_NAME=$(lsb_release -si)
    set -e

    if [ -z "${HOST_DISTRO_NAME}" ]
    then
      echo "Please install the lsb core package and rerun."
      HOST_DISTRO_NAME="Linux"
    fi

    if [ "${HOST_MACHINE}" == "x86_64" ]
    then
      HOST_BITS="64"
    elif [ "${HOST_MACHINE}" == "i686" ]
    then
      HOST_BITS="32"
    else
      echo "Unknown uname -m ${HOST_MACHINE}"
      exit 1
    fi

    HOST_DISTRO_LC_NAME=$(echo ${HOST_DISTRO_NAME} | tr "[:upper:]" "[:lower:]")

  else
    echo "Unknown uname ${HOST_UNAME}"
    exit 1
  fi

  echo
  echo "Running on ${HOST_DISTRO_NAME} ${HOST_BITS}-bit."

  GROUP_ID=$(id -g)
  USER_ID=$(id -u)
}

# -----------------------------------------------------------------------------

function host_prepare_prerequisites() 
{
  if [ "${HOST_UNAME}" == "Darwin" ]
  then
    local hb_folder="${HOME}/opt/homebrew/xbb"
    
    local must_install=""
    # Check local Homebrew.
    if [ ! -d "${hb_folder}" ]
    then
      must_install="y"
    else

      PATH="${hb_folder}/bin":${PATH}
      export PATH

      echo
      echo "Checking Homebrew in '${hb_folder}'..."
      set +e
      brew --version | grep 'Homebrew '
      if [ $? -ne 0 ]
      then
        must_install="y"
      fi
      set -e
      
    fi

    if [ -n "${must_install}" ]
    then

      echo
      echo "Please install the Homebrew XBB and rerun."
      exit 1

    fi

    if true
    then

      local tl_folder="$HOME/opt/texlive"

      must_install=""
      # Check local TeX Live.
      if [ ! -d "${tl_folder}" ]
      then
        must_install="y"
      else

        PATH="${tl_folder}/bin/x86_64-darwin":$PATH
        export PATH

        echo
        echo "Checking TeX Live in '${tl_folder}'..."
        set +e
        tex --version | grep 'TeX 3'
        if [ $? != 0 ]
        then
          must_install="y"
        fi
        set -e

      fi

      if [ -n "${must_install}" ]
      then

        echo
        echo "Please install TeX Live and rerun."
        echo "Alternatively restart the build script using '--without-pdf'."
        # echo 
        # echo "mkdir -p \${HOME}/opt"
        # echo "git clone https://github.com/ilg-ul/opt-install-scripts \${HOME}/opt/install-scripts.git"
        # echo "bash \${HOME}/opt/install-scripts.git/install-texlive.sh"
        exit 1

      fi

    fi # -z "${no_pdf}"
  fi # "${HOST_UNAME}" == "Darwin"

  # The folder that caches all downloads is in HOME
  if [ "$(uname)" == "Darwin" ] 
  then
    HOST_CACHE_FOLDER_PATH=${HOST_CACHE_FOLDER_PATH:-"${HOME}/Library/Caches/XBB"}
  else
    HOST_CACHE_FOLDER_PATH=${HOST_CACHE_FOLDER_PATH:-"${HOME}/.caches/XBB"}
  fi
  CONTAINER_CACHE_FOLDER_PATH="/Host/Caches/XBB"

  # The host script will pass to the container script
  # various environment variables.
  HOST_DEFINES_SCRIPT_PATH="${HOST_WORK_FOLDER_PATH}/build.git/scripts/host-defs-source.sh"

  DEPLOY_FOLDER_NAME=${DEPLOY_FOLDER_NAME:-"deploy"}
}

# -----------------------------------------------------------------------------

function host_prepare_docker() 
{
  echo
  echo "Checking Docker..."

  set +e
  docker --version
  if [ $? != 0 ]
  then
    echo "Please start docker daemon and rerun."
    echo "If not installed, see https://docs.docker.com/installation/."
    exit 1
  fi
  set -e
}

# -----------------------------------------------------------------------------

function host_build_target() 
{
  if [ -n "${DEBUG}" ]
  then
    echo "host_build_target $@ started."
  fi

  message="$1"
  shift

  echo
  echo "================================================================================"
  echo "=== ${message}"

  local container_script_path=""
  local target_os=""
  local target_bits="-"
  # If the docker image is not set, it is a native build.
  local docker_image=""
  local build_binaries_path=""
  local env_file=""

  while [ $# -gt 0 ]
  do

    case "$1" in
      --script)
        container_script_path="$2"
        shift 2
        ;;

      --target-os)
        target_os="$2"
        shift 2
        ;;

      --target-bits)
        target_bits="$2"
        shift 2
        ;;

      --docker-image)
        docker_image="$2"
        shift 2
        ;;

      --env-file)
        env_file="$2"
        shift 2
        ;;

      --)
        shift
        break
        ;;

      *)
        echo "Unknown option $1, exit."
        exit 1

    esac

  done

  # The remaining $@ options will be passed to the inner script.
  if [ -n "${DEBUG}" ]
  then
    echo $@
  fi

  # ---------------------------------------------------------------------------

  if [ \( -z "${target_os}" \) -a \( -n "${HOST_UNAME}" \) ]
  then
    # Build native
    if [ "${HOST_UNAME}" == "Darwin" ]
    then
      target_os="macos"
      target_bits="-"
    elif [ "${HOST_UNAME}" == "Linux" ]
    then
      target_os="linux"
      target_bits="-"
    else
      echo "Unsupported host ${HOST_UNAME}, exit."
      exit 1
    fi
  fi

  mkdir -p "$(dirname "${HOST_DEFINES_SCRIPT_PATH}")"
  echo "${RELEASE_VERSION}" >"$(dirname "${HOST_DEFINES_SCRIPT_PATH}")"/VERSION

  rm -rf "${HOST_DEFINES_SCRIPT_PATH}"
  touch "${HOST_DEFINES_SCRIPT_PATH}"

  echo "RELEASE_VERSION=\"${RELEASE_VERSION}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"

  echo "DISTRIBUTION_FILE_DATE=\"${DISTRIBUTION_FILE_DATE}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"

  echo "TARGET_OS=\"${target_os}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"
  echo "TARGET_BITS=\"${target_bits}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"
  echo "HOST_UNAME=\"${HOST_UNAME}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"
  echo "GROUP_ID=\"${GROUP_ID}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"
  echo "USER_ID=\"${USER_ID}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"

  echo "HOST_WORK_FOLDER_PATH=\"${HOST_WORK_FOLDER_PATH}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"
  echo "CONTAINER_WORK_FOLDER_PATH=\"${CONTAINER_WORK_FOLDER_PATH}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"

  echo "HOST_CACHE_FOLDER_PATH=\"${HOST_CACHE_FOLDER_PATH}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"
  echo "CONTAINER_CACHE_FOLDER_PATH=\"${CONTAINER_CACHE_FOLDER_PATH}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"

  echo "DEPLOY_FOLDER_NAME=\"${DEPLOY_FOLDER_NAME}\"" >>"${HOST_DEFINES_SCRIPT_PATH}"

  if [ -z "${docker_image}" ]
  then

    host_run_native_script \
      --script "${container_script_path}" \
      --env-file "${env_file}" \
      -- \
      $@

  else

    host_run_docker_script \
      --script "${container_script_path}" \
      --docker-image "${docker_image}" \
      --docker-container-name "${APP_LC_NAME}-${target_os}${target_bits}-build" \
      --env-file "${env_file}" \
      --host-uname "${HOST_UNAME}" \
      -- \
      $@    

  fi

  if [ -n "${DEBUG}" ]
  then
    echo "host_build_target ${rest[@]-} completed."
  fi
}

# -----------------------------------------------------------------------------

function host_run_native_script() 
{
  local local_script=""
  local env_file=""

  while [ $# -gt 0 ]
  do

    case "$1" in

      --script)
        local_script="$2"
        shift 2
        ;;

      --env-file)
        env_file="$2"
        shift 2
        ;;

      --)
        shift
        break
        ;;

      *)
        echo "Unknown option $1, exit."
        exit 1

    esac

  done

  # The remaining $@ options will be passed to the inner script.

  echo
  echo "Running script \"$(basename "${local_script}")\" natively..."

  # Run the inner script in a local sub-shell, possibly with the 
  # custom environment.
  (
    if [ -n "${env_file}" -a -f "${env_file}" ]
    then
      source "${env_file}"
    fi
    /bin/bash ${DEBUG} "${local_script}" $@
  )
}

# -----------------------------------------------------------------------------

function host_run_docker_script() 
{
  local docker_script=""
  local docker_image=""
  local docker_container_name=""
  local host_uname=""
  local env_file=""
  local opt_env_file=

  while [ $# -gt 0 ]
  do

    case "$1" in

      --script)
        docker_script="$2"
        shift 2
        ;;

      --docker-image)
        docker_image="$2"
        shift 2
        ;;

      --docker-container-name)
        docker_container_name="$2"
        shift 2
        ;;

      --host-uname)
        host_uname="$2"
        shift 2
        ;;

      --env-file)
        env_file="$2"
        shift 2
        ;;

      --)
        shift
        break
        ;;

      *)
        echo "Unknown option $1, exit."
        exit 1

    esac

  done

  set +e
  # Remove a possible previously crashed container.
  docker rm --force "${docker_container_name}" > /dev/null 2> /dev/null
  set -e

  echo
  echo "Running script \"$(basename "${docker_script}")\" inside docker image \"${docker_image}\"..."

  # Run the inner script in a fresh Docker container.
  if [ -n "${env_file}" -a -f "${env_file}" ]
  then

    docker run \
      --name="${docker_container_name}" \
      --tty \
      --hostname "docker" \
      --workdir="/root" \
      --volume="${HOST_WORK_FOLDER_PATH}/:${CONTAINER_WORK_FOLDER_PATH}" \
      --volume="${HOST_CACHE_FOLDER_PATH}/:${CONTAINER_CACHE_FOLDER_PATH}" \
      --env-file="${env_file}" \
      ${docker_image} \
      /bin/bash ${DEBUG} "${docker_script}" \
        $@

  else

    docker run \
      --name="${docker_container_name}" \
      --tty \
      --hostname "docker" \
      --workdir="/root" \
      --volume="${HOST_WORK_FOLDER_PATH}/:${CONTAINER_WORK_FOLDER_PATH}" \
      --volume="${HOST_CACHE_FOLDER_PATH}/:${CONTAINER_CACHE_FOLDER_PATH}" \
      ${docker_image} \
      /bin/bash ${DEBUG} "${docker_script}" \
        $@

  fi

  # Remove the container.
  echo "Docker container \"$(docker rm --force "${docker_container_name}")\" removed."
}

# -----------------------------------------------------------------------------

function host_show_sha() {

  if [ -d "${HOST_WORK_FOLDER_PATH}/${DEPLOY_FOLDER_NAME}" ]
  then
    echo
    set +e
    cat "${HOST_WORK_FOLDER_PATH}/${DEPLOY_FOLDER_NAME}/"*.sha
    set -e
  fi
}

# -----------------------------------------------------------------------------

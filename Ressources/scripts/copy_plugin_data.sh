CURRENT_DIR="$(pwd)"

if [ -n "$1" ]
then
    GITHUB_FILES_PATH="$1"

    echo "/////// Creating folders"
    mkdir -p "${HOME}/.ssabug/choppah/config"
    mkdir -p "${HOME}/.ssabug/choppah/presets"

    echo "/////// Copying config & skin files ..."
    cp "${GITHUB_FILES_PATH}/Ressources/config_files/config.xml" "${HOME}/.ssabug/choppah/config/"
    cp -r "${GITHUB_FILES_PATH}/Ressources/skins" "${HOME}/.ssabug/choppah/"

    echo "/////// Finished"

else
    echo "Error : no git hub files folder provided as argument"
fi




CURRENT_DIR="$(pwd)"

if [ -n "$1" ]
then
    GITHUB_FILES_PATH="$1"

    echo "/////// Creating folders"
    mkdir -p "${HOME}/.ssabug/choppah/config"
    mkdir -p "${HOME}/.ssabug/choppah/presets"
    mkdir -p "${HOME}/.ssabug/choppah/skins/default/images"

    echo "/////// Copying files ..."
    cp "${GITHUB_FILES_PATH}/Ressources/config_files/config.xml" "${HOME}/.ssabug/choppah/config/"
    cp "${GITHUB_FILES_PATH}/Ressources/config_files/skin.xml" "${HOME}/.ssabug/choppah/skins/default/"
    cp "${GITHUB_FILES_PATH}/Ressources/images/stepseq_on.png" "${HOME}/.ssabug/choppah/skins/default/images/"
    cp "${GITHUB_FILES_PATH}/Ressources/images/stepseq_off.png" "${HOME}/.ssabug/choppah/skins/default/images/"
    cp "${GITHUB_FILES_PATH}/Ressources/images/banner_4.png" "${HOME}/.ssabug/choppah/skins/default/images/"

    echo "/////// Finished"

else
    echo "Error : no git hub files folder provided as argument"
fi




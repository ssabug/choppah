CURRENT_DIR="$(pwd)"

if [ -n "$1" ]
then
    GITHUB_FILES_PATH="$1"

    echo "/////// Creating folders"
    mkdir -p "${HOME}/.ssabug/choppah/config"
    mkdir -p "${HOME}/.ssabug/choppah/presets"
    mkdir -p "${HOME}/.ssabug/choppah/skins/"

    echo "/////// Copying config & skin files ..."
    cp "${GITHUB_FILES_PATH}/Ressources/config_files/config.xml" "${HOME}/.ssabug/choppah/config/"
    cp -r "${GITHUB_FILES_PATH}/Ressources/skins/default" "${HOME}/.ssabug/choppah/skins/"
    cp -r "${GITHUB_FILES_PATH}/Ressources/skins/red" "${HOME}/.ssabug/choppah/skins/"
    cp -r "${GITHUB_FILES_PATH}/Ressources/skins/green" "${HOME}/.ssabug/choppah/skins/"
    cp -r "${GITHUB_FILES_PATH}/Ressources/skins/blue" "${HOME}/.ssabug/choppah/skins/"
    cp -r "${GITHUB_FILES_PATH}/Ressources/skins/yellow" "${HOME}/.ssabug/choppah/skins/"
    cp -r "${GITHUB_FILES_PATH}/Ressources/skins/turquoise" "${HOME}/.ssabug/choppah/skins/"

    echo "/////// Finished"

else
    echo "Error : no git hub files folder provided as argument"
fi




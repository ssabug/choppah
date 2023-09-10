BASE_DIR="${HOME}/dev/choppah"
CURRENT_DIR="$(pwd)"

if [ -n "$1" ]
then
    COMMIT_MSG="$1"

    echo "/////// MOVING TO PROJECT DIR ${BASE_DIR}"
    cd "${BASE_DIR}"

    echo "/////// ADDING ALL FILES"
    git add --all

    echo "/////// COMMITTING ..."
    git commit -am "${COMMIT_MSG}"

    echo "/////// PUSHING ..."
    git push

    echo "/////// coming back to ${CURRENT_DIR}"
    cd "${CURRENT_DIR}"
else
    echo "Error : no git commit text supplied as argument"
fi




#!/usr/bin/env bash
# Build GitHub Pages branch automatically.
## STARTING STATE: Must be in the branch master, with no uncommitted changes.
## You've run the tests on release mode, and they run successfully!

# Exit script on first error:
set -e

# Get the current commit
REPO=brenns10/nosj
COMMIT=$(git rev-parse HEAD)
WORK="$(pwd)"

# Build, test, coverage
rm -rf build
meson build -Db_coverage=true
ninja -C build test
ninja -C build coverage-html

# Generate documentation.
doxygen
make -C doc html

# In a new directory, clone the gh-pages branch.
rm -rf /tmp/gh-pages
if [ -z "$GH_TOKEN" ]; then
	git clone -b gh-pages git@github.com:$REPO /tmp/gh-pages
else
	git clone -b gh-pages "https://$GH_TOKEN@github.com/$REPO" /tmp/gh-pages
fi
pushd /tmp/gh-pages
rm -rf doc
rm -rf cov

# Update git configuration so I can push.
if [ "$1" != "dry" ]; then
    # Update git config.
    git config --local user.name "Pages Builder"
    git config --local user.email "example@example.com"
fi

# Copy the docs and coverage results.
rm -rf doc
cp -R $WORK/doc/_build/html doc
cp -R $WORK/build/meson-logs/coveragereport ./cov

# Add and commit changes.
git add -A .
git commit -m "[ci skip] Autodoc commit for $COMMIT."
if [ "$1" != "dry" ]; then
    git push -q origin gh-pages
fi
popd
rm -rf /tmp/gh-pages

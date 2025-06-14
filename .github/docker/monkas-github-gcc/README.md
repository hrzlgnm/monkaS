# monkas-github-gcc Docker image

## Updating the Docker image

> [!NOTE]
> This is usually not needed to be done manually, as the Docker image is automatically built and pushed
> by the GitHub Actions workflow defined in the `.github/workflows/docker-build.yml` file.

Pick a version number for the new Docker image (e.g. `v3`), then run the
following commands:

    $ docker build --tag ghcr.io/hrzlgnm/monkas-github-gcc:VERSION_NUMBER_HERE .github/docker/monkas-github-gcc/
    $ docker login ghcr.io -u YOUR_GITHUB_USER_NAME_HERE
    $ docker push ghcr.io/hrzlgnm/monkas-github-gcc:VERSION_NUMBER_HERE

Then, change the container tag in each workflow file in the .github/workflows/
directory to refer to your new version.

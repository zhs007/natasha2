FROM centos:7

USER root

RUN yum --nogpgcheck install centos-release-scl -y \
    && yum --nogpgcheck install devtoolset-7-toolchain -y
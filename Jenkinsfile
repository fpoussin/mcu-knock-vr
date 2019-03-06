pipeline {
  agent {
    docker {
      image 'fpoussin/jenkins:ubuntu-18.04-arm'
    }

  }
  stages {
    stage('submodules') {
      steps {
        sh '''git submodule sync
git submodule update --init
'''
      }
    }
    stage('build') {
      steps {
        sh '''cd $WORKSPACE/code/cm4-dsp-lib
make
cd ..'''
        sh '''cd $WORKSPACE/code
make'''
      }
    }
    stage('clean') {
      steps {
        sh '''cd $WORKSPACE/code
make clean'''
      }
    }
  }
}
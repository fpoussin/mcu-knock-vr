pipeline {
  agent {
    docker {
      image 'fpoussin/jenkins:ubuntu-16.10-arm'
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
  }
}
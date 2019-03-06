pipeline {
  agent {
    docker {
      image 'fpoussin/jenkins:ubuntu-16.10-arm'
    }

  }
  stages {
    stage('submodules') {
      steps {
        sh '''git submodule update --init
'''
      }
    }
    stage('build') {
      steps {
        sh 'make'
      }
    }
  }
}
pipeline {
    agent none
    environment {
        DOCKER_IMAGE='lmc/build'
    }
    stages {
        stage('Gitlab Pending') {
            steps {
                echo 'Notifying Gitlab'
                updateGitlabCommitStatus name: 'build', state: 'pending'
            }
        }
        stage('Docker Image') {
            agent any
            steps {
                echo '-- Docker Image --'
                sh 'docker build -f Dockerfile -t ${DOCKER_IMAGE} .'
            }
        }
        stage('Build') {
            agent {
                docker { image "${DOCKER_IMAGE}" }
            }
            steps {
                echo '-- Starting Build --'
                make
            }
        }
        stage('Test') {
            agent {
                docket { image "${DOCKER_IMAGE}" }
            }
            steps {
                echo '-- Running Tests --'
                make test
            }
        }
        stage('Gitlab Success') {
            steps {
                echo '-- Notifying Gitlab --'
                updateGitlabCommitStatus name: 'build', state: 'success'
            }
        }
    }
    post {
        always {
            node(null) {
                echo '-- Removing dangling Docker images --'
                sh 'docker system prune -f'

                echo '-- Cleaning up --'
                deleteDir()
            }
        }
    }
}

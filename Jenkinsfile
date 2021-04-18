pipeline {
    agent none
    environment {
        DOCKER_IMAGE='cldm/build'
        ARTIFACT_DIR='artifacts'
        NFUZZERRUNS=30
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

                echo 'Generating empty mockups.h file'
                sh 'touch cldm/mockups.h'

                sh 'make -j$(nproc)'
            }
        }
        stage('Test') {
            when {
                beforeAgent true
                expression {
                    return env.TARGET != 'fuzz'
                }
            }
            agent {
                docker { image "${DOCKER_IMAGE}" }
            }
            steps {
                echo '-- Running Tests --'
                sh '''
                    make -j$(nproc) cldmtest
                    LD_LIBRARY_PATH=. valgrind ./cldmtest
                    make functional
                '''
            }
        }
        stage('Fetch Corpora') {
            when {
                beforeAgent true
                expression {
                    return env.TARGET == 'fuzz'
                }
            }
            agent {
                docker { image "${DOCKER_IMAGE}" }
            }
            steps {
                echo 'Copying existing corpora'
                sh 'mkdir -p ${ARTIFACT_DIR}/{current,prev}'
                copyArtifacts filter: "${ARTIFACT_DIR}/corpora.zip", projectName: "${JOB_NAME}", fingerprintArtifacts: true, optional: true
                script {
                    if(fileExists("${ARTIFACT_DIR}/corpora.zip")) {
                        unzip zipFile: "${ARTIFACT_DIR}/corpora.zip", dir: "${ARTIFACT_DIR}/prev"
                    }
                    else {
                        echo 'No corpora found'
                    }
                }
            }
        }
        stage('Build Fuzzer') {
            when {
                beforeAgent true
                expression {
                    return env.TARGET == 'fuzz'
                }
            }
            agent {
                docker { image "${DOCKER_IMAGE}" }
            }
            steps {
                echo '-- Building Fuzzer --'
                sh 'make -j$(nproc) fuzz'
            }
        }
        stage('Merge Corpora') {
            when {
                beforeAgent true
                expression {
                    return env.TARGET == 'fuzz'
                }
            }
            agent {
                docker { image "${DOCKER_IMAGE}" }
            }
            steps {
                echo 'Merging corpora'
                sh 'make fuzzmerge CORPORA=${ARTIFACT_DIR}/prev'
            }
        }
        stage('Fuzz') {
            when {
                beforeAgent true
                expression {
                    return env.TARGET == 'fuzz'
                }
            }
            agent {
                docker { image "${DOCKER_IMAGE}" }
            }
            steps {
                echo '-- Running Fuzzer --'
                sh '''
                    for i in $(seq 1 ${NFUZZERRUNS}) ; do
                        make fuzzrun
                    done
                '''
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
                script {
                    if(env.TARGET == 'fuzz') {
                        sh 'rm -rf ${ARTIFACT_DIR}/corpora.zip'

                        zip zipFile: "$ARTIFACT_DIR/corpora.zip", archive:true, dir: "test/fuzz/corpora", overwrite: false
                        archiveArtifacts artifacts: "${ARTIFACT_DIR}/corpora.zip", fingerprint: true
                    }
                }

                echo '-- Removing dangling Docker images --'
                sh 'docker system prune -f'

                echo '-- Cleaning up --'
                deleteDir()
            }
        }
    }
}

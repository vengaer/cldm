class config {
    public static final int NFUZZRUNS = 30
}

fuzztargets = ['avx2_memcmp', 'avx2_memcpy', 'avx2_memset', 'avx2_scan_lt', 'avx2_strscpy', 'rbtree', 'argp', 'hash']
ccs = ['gcc', 'clang']

pipeline {
    agent none
    environment {
        DOCKER_IMAGE='cldm/build'
        ARTIFACT_DIR='artifacts'
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
                sh "docker build -f Dockerfile -t ${DOCKER_IMAGE} ."
            }
        }
        stage('Build') {
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
                script {
                    ccs.each { cc ->
                        echo "-- Starting ${cc} Build --"
                        sh "CC=${cc} make -B -j\$(nproc)"
                    }
                }
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
                script {
                    ccs.each { cc ->
                        echo "-- Running Single Threaded ${cc} Tests --"
                        sh "CC=${cc} make -B vgcheck"

                        echo "-- Running Parallel ${cc} Tests --"
                        sh "CC=${cc} make -B vgparunit"
                    }
                }
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
            agent {
                docker { image "${DOCKER_IMAGE}" }
            }
            steps {
                echo '-- Building Fuzzer --'
                sh 'make -j$(nproc) fuzz'
            }
        }
        stage('Merge Corpora') {
            agent {
                docker { image "${DOCKER_IMAGE}" }
            }
            steps {
                script {
                    fuzztargets.each { target ->
                        echo "Merging ${target} corpora"
                        if(fileExists("${ARTIFACT_DIR}/prev/${target}")) {
                            sh "CLDM_FUZZTARGET=${target} make fuzzmerge CORPORA=${ARTIFACT_DIR}/prev/${target}"
                        }
                    }
                }
            }
        }
        stage('Dynamic Fuzzing') {
            agent {
                docker { image "${DOCKER_IMAGE}" }
            }
            steps {
                script {
                    fuzztargets.each { target ->
                        stage("Fuzz ${target}") {
                            if(env.TARGET == 'fuzz') {
                                nruns = config.NFUZZRUNS
                                fuzztime = 240
                            }
                            else {
                                nruns = 1
                                fuzztime = 25
                            }
                            for(int i = 0; i < nruns; i++) {
                                echo "Fuzzing ${target} ${i + 1}/${nruns}"
                                sh "CLDM_FUZZTARGET=${target} make fuzzrun FUZZTIME=${fuzztime}"
                            }
                        }
                    }
                }
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
                    if(fileExists("${ARTIFACT_DIR}/corpora.zip")) {
                        sh "rm ${ARTIFACT_DIR}/corpora.zip"
                    }

                    zip zipFile: "$ARTIFACT_DIR/corpora.zip", archive:true, dir: "test/fuzz/corpora", overwrite: false
                    archiveArtifacts artifacts: "${ARTIFACT_DIR}/corpora.zip", fingerprint: true
                }

                echo '-- Removing dangling Docker images --'
                sh 'docker system prune -f'

                echo '-- Cleaning up --'
                deleteDir()
            }
        }
    }
}

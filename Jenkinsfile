class config {
    public static final int CI_FUZZTIME = 25
    public static final int FUZZTIME = 240
    public static final int CI_NFUZZRUNS = 1
    public static final int NFUZZRUNS = 30
}

fuzztargets = ['avx2_memcmp', 'avx2_memcpy', 'avx2_memset', 'avx2_memswp', 'avx2_scan_lt', 'avx2_strcmp', 'avx2_strlen', 'avx2_strrchr', 'avx2_strscpy', 'rbtree', 'argp', 'hash']
ccs = ['gcc', 'clang']

valid_types = [ 'fuzz' ]
fuzztime = config.CI_FUZZTIME
nfuzzruns = config.CI_NFUZZRUNS

pipeline {
    agent none
    environment {
        DOCKER_IMAGE='cldm/build'
        ARTIFACT_DIR='artifacts'
    }
    stages {
        stage('Validate Environment') {
            agent any
            steps {
                script {
                    if(env.TYPE) {
                        if(!valid_types.contains(env.TYPE)) {
                            error("Invalid type ${TYPE}")
                        }
                        if(env.TYPE == 'fuzz') {
                            fuzztime = config.FUZZTIME
                            nfuzzruns = config.NFUZZRUNS
                        }
                    }
                    if(env.FUZZTARGET && !fuzztargets.contains(env.FUZZTARGET)) {
                        error("Invalid fuzzer target ${FUZZTARGET}")
                    }
                    if(env.FUZZTIME) {
                        if(!env.FUZZTIME.isInteger()) {
                            error("FUZZTIME must be an integer")
                        }
                        fuzztime = env.FUZZTIME.toInteger()
                    }
                    if(env.NFUZZRUNS) {
                        if(!env.NFUZZRUNS.isInteger()) {
                            error("NFUZZRUNS must be an integer")
                        }
                        nfuzzruns = env.NFUZZRUNS.toInteger()
                    }
                }
            }
        }
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
        stage('Build Host') {
            when {
                beforeAgent true
                expression {
                    return env.TYPE != 'fuzz'
                }
            }
            agent {
                docker {
                    image "${DOCKER_IMAGE}"
                }
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
        stage('Build aarch64') {
            when {
                beforeAgent true
                expression {
                    return env.TYPE != 'fuzz'
                }
            }
            agent {
                docker {
                    image "${DOCKER_IMAGE}"
                }
            }
            steps {
                sh 'make -B -j\$(nproc) ARCH=aarch64'
            }
        }
        stage('Dynamic Test') {
            when {
                beforeAgent true
                expression {
                    return env.TYPE != 'fuzz'
                }
            }
            agent {
                docker {
                    image "${DOCKER_IMAGE}"
                }
            }
            steps {
                script {
                    ccs.each { cc ->
                        stage("Test ${cc}") {
                            echo "-- Running Single Threaded ${cc} Tests --"
                            sh "CC=${cc} make -B vgcheck"

                            echo "-- Running Parallel ${cc} Tests --"
                            sh "CC=${cc} make -B vgparunit"
                        }
                    }
                }
            }
        }
        stage('Fetch Corpora') {
            agent {
                docker {
                    image "${DOCKER_IMAGE}"
                }
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
                docker {
                    image "${DOCKER_IMAGE}"
                }
            }
            steps {
                echo '-- Building Fuzzer --'
                sh 'make -j$(nproc) fuzz'
            }
        }
        stage('Merge Corpora') {
            agent {
                docker {
                    image "${DOCKER_IMAGE}"
                }
            }
            steps {
                script {
                    fuzztargets.each { target ->
                        if(fileExists("${ARTIFACT_DIR}/prev/${target}")) {
                            echo "Merging ${target} corpora"
                            sh "CLDM_FUZZTARGET=${target} make fuzzmerge CORPORA=${ARTIFACT_DIR}/prev/${target}"
                        }
                        else {
                            echo "No corpora found for ${target}"
                        }
                    }
                }
            }
        }
        stage('Dynamic Fuzzing') {
            agent {
                docker {
                    image "${DOCKER_IMAGE}"
                }
            }
            steps {
                script {
                    echo "-- Running ${nfuzzruns} iterations, each with duration ${fuzztime}s, per fuzz target --"
                    echo "Expected duration per target: ${fuzztime * nfuzzruns / 60}m, ${(fuzztime * nfuzzruns) % 60}s"
                    fuzztargets.each { target ->
                        stage("Fuzz ${target}") {
                            if(!env.FUZZTARGET || env.FUZZTARGET == "${target}") {
                                for(int i = 0; i < nfuzzruns; i++) {
                                    echo "Fuzzing ${target} ${i + 1}/${nfuzzruns}"
                                    sh "CLDM_FUZZTARGET=${target} make fuzzrun FUZZTIME=${fuzztime}"
                                }
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

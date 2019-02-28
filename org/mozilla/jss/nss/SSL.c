#include <nspr.h>
#include <nss.h>
#include <ssl.h>
#include <limits.h>
#include <stdint.h>
#include <jni.h>

#include "java_ids.h"
#include "jssutil.h"
#include "pk11util.h"
#include "PRFDProxy.h"

#include "_jni/org_mozilla_jss_nss_SSL.h"

jobject JSS_NewSecurityStatusResult(JNIEnv *env, int on, char *cipher,
    int keySize, int secretKeySize, char *issuer, char *subject)
{
    jclass resultClass;
    jmethodID constructor;
    jobject result = NULL;
    jbyteArray cipher_java = NULL;
    jbyteArray issuer_java = NULL;
    jbyteArray subject_java = NULL;

    PR_ASSERT(env != NULL);

    resultClass = (*env)->FindClass(env, SECURITY_STATUS_CLASS_NAME);
    if (resultClass == NULL) {
        ASSERT_OUTOFMEM(env);
        goto finish;
    }

    constructor = (*env)->GetMethodID(env, resultClass, PLAIN_CONSTRUCTOR,
        SECURITY_STATUS_CONSTRUCTOR_SIG);
    if (constructor == NULL) {
        ASSERT_OUTOFMEM(env);
        goto finish;
    }

    if (cipher) {
        cipher_java = JSS_ToByteArray(env, cipher, strlen(cipher));
    }

    if (issuer) {
        issuer_java = JSS_ToByteArray(env, issuer, strlen(issuer));
    }

    if (subject) {
        subject_java = JSS_ToByteArray(env, subject, strlen(subject));
    }

    result = (*env)->NewObject(env, resultClass, constructor, on, cipher_java,
        keySize, secretKeySize, issuer_java, subject_java);

finish:
    return result;
}

JNIEXPORT jobject JNICALL
Java_org_mozilla_jss_nss_SSL_ImportFD(JNIEnv *env, jclass clazz, jobject model,
    jobject fd)
{
    PRFileDesc *result = NULL;
    PRFileDesc *real_model = NULL;
    PRFileDesc *real_fd = NULL;

    PR_ASSERT(env != NULL);

    /* Note: NSS calling semantics state that either model or fd can be
     * NULL; so when the Java Object is not-NULL, dereference it. */
    if (model != NULL && JSS_PR_getPRFileDesc(env, model, &real_model) != PR_SUCCESS) {
        return NULL;
    }

    if (fd != NULL && JSS_PR_getPRFileDesc(env, fd, &real_fd) != PR_SUCCESS) {
        return NULL;
    }

    result = SSL_ImportFD(real_model, real_fd);

    return JSS_PR_wrapPRFDProxy(env, &result);
}

JNIEXPORT int JNICALL
Java_org_mozilla_jss_nss_SSL_OptionSet(JNIEnv *env, jclass clazz, jobject fd,
    jint option, jint val)
{
    PRFileDesc *real_fd = NULL;

    PR_ASSERT(env != NULL && fd != NULL);

    if (JSS_PR_getPRFileDesc(env, fd, &real_fd) != PR_SUCCESS) {
        return SECFailure;
    }

    return SSL_OptionSet(real_fd, option, val);
}

JNIEXPORT int JNICALL
Java_org_mozilla_jss_nss_SSL_SetURL(JNIEnv *env, jclass clazz, jobject fd,
    jstring url)
{
    PRFileDesc *real_fd = NULL;
    char *real_url = NULL;

    PR_ASSERT(env != NULL && fd != NULL);

    if (JSS_PR_getPRFileDesc(env, fd, &real_fd) != PR_SUCCESS) {
        return SECFailure;
    }

    real_url = (char *)(*env)->GetStringUTFChars(env, url, NULL);
    if (real_url == NULL) {
        return SECFailure;
    }

    return SSL_SetURL(real_fd, real_url);
}

JNIEXPORT jobject JNICALL
Java_org_mozilla_jss_nss_SSL_SecurityStatus(JNIEnv *env, jclass clazz,
    jobject fd)
{
    PRFileDesc *real_fd = NULL;
    int on;
    char *cipher;
    int keySize;
    int secretKeySize;
    char *issuer;
    char *subject;

    PR_ASSERT(env != NULL && fd != NULL);

    if (JSS_PR_getPRFileDesc(env, fd, &real_fd) != PR_SUCCESS) {
        return NULL;
    }

    if (SSL_SecurityStatus(real_fd, &on, &cipher, &keySize, &secretKeySize, &issuer, &subject) != SECSuccess) {
        return NULL;
    }

    return JSS_NewSecurityStatusResult(env, on, cipher, keySize, secretKeySize,
        issuer, subject);
}

JNIEXPORT int JNICALL
Java_org_mozilla_jss_nss_SSL_ResetHandshake(JNIEnv *env, jclass clazz,
    jobject fd, jboolean asServer)
{
    PRFileDesc *real_fd = NULL;

    PR_ASSERT(env != NULL && fd != NULL);

    if (JSS_PR_getPRFileDesc(env, fd, &real_fd) != PR_SUCCESS) {
        return SECFailure;
    }

    return SSL_ResetHandshake(real_fd, asServer);
}

JNIEXPORT int JNICALL
Java_org_mozilla_jss_nss_SSL_ForceHandshake(JNIEnv *env, jclass clazz,
    jobject fd)
{
    PRFileDesc *real_fd = NULL;

    PR_ASSERT(env != NULL && fd != NULL);

    if (JSS_PR_getPRFileDesc(env, fd, &real_fd) != PR_SUCCESS) {
        return SECFailure;
    }

    return SSL_ForceHandshake(real_fd);
}

JNIEXPORT int JNICALL
Java_org_mozilla_jss_nss_SSL_ConfigSecureServer(JNIEnv *env, jclass clazz,
    jobject fd, jobject cert, jobject key, jint kea)
{
    PRFileDesc *real_fd = NULL;
    CERTCertificate *real_cert = NULL;
    SECKEYPrivateKey *real_key = NULL;

    PR_ASSERT(env != NULL && fd != NULL);

    if (JSS_PR_getPRFileDesc(env, fd, &real_fd) != PR_SUCCESS) {
        return SECFailure;
    }

    if (JSS_PK11_getCertPtr(env, cert, &real_cert) != PR_SUCCESS) {
        return SECFailure;
    }

    if (JSS_PK11_getPrivKeyPtr(env, key, &real_key) != PR_SUCCESS) {
        return SECFailure;
    }

    return SSL_ConfigSecureServer(real_fd, real_cert, real_key, kea);
}

JNIEXPORT int JNICALL
Java_org_mozilla_jss_nss_SSL_ConfigServerSessionIDCache(JNIEnv *env, jclass clazz,
    jint maxCacheEntries, jlong timeout, jlong ssl3_timeout, jstring directory)
{
    const char *dir_path;
    SECStatus ret = SECFailure;

    PR_ASSERT(env != NULL);

    dir_path = JSS_RefJString(env, directory);

    ret = SSL_ConfigServerSessionIDCache(maxCacheEntries, timeout,
        ssl3_timeout, dir_path);

    JSS_DerefJString(env, directory, dir_path);
    return ret;
}

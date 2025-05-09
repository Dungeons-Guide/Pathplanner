//
// Created by syeyoung on 5/8/25.
//

#include "kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP.h"
#include "tsp.h"

void convert(JNIEnv* env, vector<int>& dest, jintArray arr) {
    int len = env->GetArrayLength(arr);
    jint* intPtr = env->GetIntArrayElements(arr, NULL);
    for (int i = 0; i < len; i++) {
        dest.push_back(*(intPtr+i));
    }
    env->ReleaseIntArrayElements(arr, intPtr, 0);
}
void convert(JNIEnv* env, vector<bool>& dest, jbooleanArray arr) {
    int len = env->GetArrayLength(arr);
    jboolean * intPtr = env->GetBooleanArrayElements(arr, NULL);
    for (int i = 0; i < len; i++) {
        dest.push_back(*(intPtr+i));
    }
    env->ReleaseBooleanArrayElements(arr, intPtr, 0);
}
void convert(JNIEnv* env, vector<ll>& dest, jlongArray arr) {
    int len = env->GetArrayLength(arr);
    jlong * intPtr = env->GetLongArrayElements(arr, NULL);
    for (int i = 0; i < len; i++) {
        dest.push_back(*(intPtr+i));
    }
    env->ReleaseLongArrayElements(arr, intPtr, 0);
}

jlong JNICALL Java_kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP_startCoroutine(JNIEnv* env, jobject obj) {
    auto clazz = env->GetObjectClass(obj);

    NodeMapping nodeMapping;
    auto nodeType = reinterpret_cast<jintArray>(env->GetObjectField(obj, env->GetFieldID(clazz, "nodeType", "[I")));
    nodeMapping.N = env->GetArrayLength(nodeType);
    convert(env, nodeMapping.nodeType, nodeType);
    convert(env, nodeMapping.requireBitIndex, reinterpret_cast<jintArray>(env->GetObjectField(obj, env->GetFieldID(clazz, "requireIdBitMapping", "[I"))));
    convert(env, nodeMapping.orSubIndex, reinterpret_cast<jintArray>(env->GetObjectField(obj, env->GetFieldID(clazz, "orIdIdxMapping", "[I"))));
    convert(env, nodeMapping.sanity, reinterpret_cast<jbooleanArray>(env->GetObjectField(obj, env->GetFieldID(clazz, "sanity", "[Z"))));

    convert(env, nodeMapping.require, reinterpret_cast<jlongArray>(env->GetObjectField(obj, env->GetFieldID(clazz, "require", "[J"))));

    auto ors = reinterpret_cast<jobjectArray>(env ->GetObjectField(obj, env->GetFieldID(clazz, "or", "[[I")));
    int orsLen = env->GetArrayLength(ors);
    nodeMapping.orReq.resize(orsLen);
    for (int i = 0; i < orsLen; i++) {
        auto arr = reinterpret_cast<jintArray>(env->GetObjectArrayElement(ors, i));
        convert(env, nodeMapping.orReq[i], arr);
    }

    int cnt = *std::max_element(nodeMapping.nodeType.begin(), nodeMapping.nodeType.end()) - 1;
    if (cnt < 0) cnt = 0;

    nodeMapping.orMult.resize(cnt);
    nodeMapping.orCount.resize(cnt);
    std::fill(nodeMapping.orCount.begin(), nodeMapping.orCount.end(), 1);
    for (const auto &item: nodeMapping.nodeType) {
        if (item > 1) nodeMapping.orCount[item-2]++;
    }

    ll mult = 1ll;
    for (int i = 0; i < cnt; i++) {
        nodeMapping.orMult[i] = mult;
        mult *= nodeMapping.orCount[i];
    }

    auto stBitset = reinterpret_cast<jint>(env ->GetIntField(obj, env->GetFieldID(clazz, "stBitset", "I")));
    double startX = env->GetDoubleField(obj, env ->GetFieldID(clazz, "startX", "D"));
    double startY = env->GetDoubleField(obj, env ->GetFieldID(clazz, "startY", "D"));
    double startZ = env->GetDoubleField(obj, env ->GetFieldID(clazz, "startZ", "D"));

//    auto method = env->GetMethodID(clazz, "evaluate", "(DDDII)Lkr/syeyoung/dungeonsguide/mod/dungeon/actions/route/DPTSP$EvalRes;");
//
//    jclass  resClazz = env->FindClass("kr/syeyoung/dungeonsguide/mod/dungeon/actions/route/DPTSP$EvalRes");
//
//    auto methodResX = env->GetFieldID(resClazz, "x", "D");
//    auto methodResY = env->GetFieldID(resClazz, "y", "D");
//    auto methodResZ = env->GetFieldID(resClazz, "z", "D");
//    auto methodResCost = env->GetFieldID(resClazz, "cost", "D");
//    auto methodResMech = env->GetFieldID(resClazz, "mechanic", "I");

    TSP* tsp = new TSP(nodeMapping, Vec3{startX, startY, startZ}, stBitset);
    return reinterpret_cast<jlong>(tsp);
}

JNIEXPORT jboolean JNICALL Java_kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP_resumeCoroutine
        (JNIEnv * env, jobject obj, jlong handle_addr, jdouble x, jdouble y, jdouble z, jint mech, jdouble cost) {
    auto* handle = reinterpret_cast<TSP*>(handle_addr);
    if (!handle) {
        return JNI_FALSE;
    }
    if (handle->routine.handle.done()) return JNI_FALSE;
    handle->awaiter.mechId = mech;
    handle->awaiter.coord = {x, y, z};
    handle->awaiter.result = cost;
    handle->awaiter.handle.resume();
    return JNI_TRUE;
}

JNIEXPORT jintArray JNICALL Java_kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP_getResult
        (JNIEnv * env, jobject obj, jlong handle_addr, jint goal) {
    auto* handle = reinterpret_cast<TSP*>(handle_addr);
    if (!handle) {
        return nullptr;
    }
    vector<int> res = handle->getSolution(goal);

    jint resArray[res.size()];
    for (int i = 0; i < res.size(); ++i)
        resArray[i] = res[i];
    auto arr = env->NewIntArray(res.size());
    env->SetIntArrayRegion(arr, 0, res.size(), resArray);
    return arr;
}
JNIEXPORT jdouble JNICALL Java_kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP_getX
        (JNIEnv *, jobject, jlong handle_addr) {
    return reinterpret_cast<TSP*>(handle_addr)->awaiter.coord[0];
}

JNIEXPORT jdouble JNICALL Java_kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP_getY
        (JNIEnv *, jobject, jlong handle_addr) {
    return reinterpret_cast<TSP*>(handle_addr)->awaiter.coord[1];
}

JNIEXPORT jdouble JNICALL Java_kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP_getZ
        (JNIEnv *, jobject, jlong handle_addr) {
    return reinterpret_cast<TSP*>(handle_addr)->awaiter.coord[2];
}

JNIEXPORT jint JNICALL Java_kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP_getNode
        (JNIEnv *, jobject, jlong handle_addr) {
    return reinterpret_cast<TSP*>(handle_addr)->awaiter.nodeId;
}

JNIEXPORT jint JNICALL Java_kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP_getMech
        (JNIEnv *, jobject, jlong handle_addr) {
    return reinterpret_cast<TSP*>(handle_addr)->awaiter.mechId;
}

JNIEXPORT void JNICALL Java_kr_syeyoung_dungeonsguide_mod_dungeon_actions_route_DPTSP_destoryCoroutine
        (JNIEnv * env, jobject obj, jlong handle_addr) {
    auto* handle = reinterpret_cast<TSP*>(handle_addr);
    delete handle;
}
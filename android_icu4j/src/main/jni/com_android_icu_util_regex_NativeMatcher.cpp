/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "NativeMatcher"

#include <memory>
#include <stdlib.h>

#include <nativehelper/JNIHelp.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include <nativehelper/ScopedStringChars.h>
#include <nativehelper/jni_macros.h>

#include <android-base/logging.h>
#include "IcuUtilities.h"
#include "MatcherState.h"
#include "ScopedJavaUnicodeString.h"
#include "unicode/parseerr.h"
#include "unicode/regex.h"

// ICU documentation: http://icu-project.org/apiref/icu4c/classRegexMatcher.html

static inline MatcherState* toMatcherState(jlong address) {
    return reinterpret_cast<MatcherState*>(static_cast<uintptr_t>(address));
}

static void NativeMatcher_free(void* address) {
    MatcherState* state = reinterpret_cast<MatcherState*>(address);
    delete state;
}

static jlong NativeMatcher_getNativeFinalizer(JNIEnv*, jclass) {
    return reinterpret_cast<jlong>(&NativeMatcher_free);
}

static jboolean NativeMatcher_findImpl(JNIEnv* env, jclass, jlong addr, jint startIndex, jintArray offsets) {
    MatcherState* state = toMatcherState(addr);
    UBool result = state->matcher()->find(startIndex, state->status());
    if (result) {
        state->updateOffsets(env, offsets);
        return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}

static jboolean NativeMatcher_findNextImpl(JNIEnv* env, jclass, jlong addr, jintArray offsets) {
    MatcherState* state = toMatcherState(addr);
    UBool result = state->matcher()->find();
    if (result) {
        state->updateOffsets(env, offsets);
        return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}

static jint NativeMatcher_groupCountImpl(JNIEnv*, jclass, jlong addr) {
    MatcherState* state = toMatcherState(addr);
    return state->matcher()->groupCount();
}

static jboolean NativeMatcher_hitEndImpl(JNIEnv*, jclass, jlong addr) {
    MatcherState* state = toMatcherState(addr);
    if (state->matcher()->hitEnd() != 0) {
        return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}

static jboolean NativeMatcher_lookingAtImpl(JNIEnv* env, jclass, jlong addr, jintArray offsets) {
    MatcherState* state = toMatcherState(addr);
    UBool result = state->matcher()->lookingAt(state->status());
    if (result) {
        state->updateOffsets(env, offsets);
        return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}

static jboolean NativeMatcher_matchesImpl(JNIEnv* env, jclass, jlong addr, jintArray offsets) {
    MatcherState* state = toMatcherState(addr);
    UBool result = state->matcher()->matches(state->status());
    if (result) {
        state->updateOffsets(env, offsets);
        return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}

static jboolean NativeMatcher_requireEndImpl(JNIEnv*, jclass, jlong addr) {
    MatcherState* state = toMatcherState(addr);
    if (state->matcher()->requireEnd() != 0) {
        return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}

static void NativeMatcher_setInputImpl(JNIEnv* env, jclass, jlong addr, jstring javaText, jint start, jint end) {
    MatcherState* state = toMatcherState(addr);
    if (state->updateInput(env, javaText)) {
        state->matcher()->region(start, end, state->status());
    }
}

static void NativeMatcher_useAnchoringBoundsImpl(JNIEnv*, jclass, jlong addr, jboolean value) {
    MatcherState* state = toMatcherState(addr);
    state->matcher()->useAnchoringBounds(value);
}

static void NativeMatcher_useTransparentBoundsImpl(JNIEnv*, jclass, jlong addr, jboolean value) {
    MatcherState* state = toMatcherState(addr);
    state->matcher()->useTransparentBounds(value);
}


static JNINativeMethod gMethods[] = {
    NATIVE_METHOD(NativeMatcher, findImpl, "(JI[I)Z"),
    NATIVE_METHOD(NativeMatcher, findNextImpl, "(J[I)Z"),
    NATIVE_METHOD(NativeMatcher, getNativeFinalizer, "()J"),
    NATIVE_METHOD(NativeMatcher, groupCountImpl, "(J)I"),
    NATIVE_METHOD(NativeMatcher, hitEndImpl, "(J)Z"),
    NATIVE_METHOD(NativeMatcher, lookingAtImpl, "(J[I)Z"),
    NATIVE_METHOD(NativeMatcher, matchesImpl, "(J[I)Z"),
    NATIVE_METHOD(NativeMatcher, requireEndImpl, "(J)Z"),
    NATIVE_METHOD(NativeMatcher, setInputImpl, "(JLjava/lang/String;II)V"),
    NATIVE_METHOD(NativeMatcher, useAnchoringBoundsImpl, "(JZ)V"),
    NATIVE_METHOD(NativeMatcher, useTransparentBoundsImpl, "(JZ)V"),
};
void register_com_android_icu_util_regex_NativeMatcher(JNIEnv* env) {
    jniRegisterNativeMethods(env, "com/android/icu/util/regex/NativeMatcher", gMethods, NELEM(gMethods));
}

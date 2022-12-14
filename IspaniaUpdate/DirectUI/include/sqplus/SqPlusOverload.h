// SqPlusOverload.h
// SqPlus function overloading support created by Katsuaki Kawachi.

#ifdef SQPLUS_OVERLOAD_DECLARATION
#undef SQPLUS_OVERLOAD_DECLARATION

template<typename Func> struct Arg;

#endif // SQPLUS_OVERLOAD_DECLARATION


#ifdef SQPLUS_OVERLOAD_IMPLEMENTATION
#undef SQPLUS_OVERLOAD_IMPLEMENTATION
private:
    class SQFuncHolder {
    private:
        template<typename T>
        class FlexArray {
        protected:
            SquirrelObject array;
        public:
            FlexArray(int size = 0) {
                this->resize(size);
            }
            int size(void) const {
                return array.Len();
            }
            void resize(int newSize) {
                if (this->size() == 0) {
                    array = SquirrelVM::CreateArray(newSize);
                } else {
                    array.ArrayResize(newSize);
                }
            }
            void push_back(const T &t) {
                this->set(this->size(), t);
            }
            void set(int index, const T &t) {
                get(index) = t;
            }
            T &get(int index) {
                if (index >= array.Len()) {
                    resize(index + 1);
                }
                SQUserPointer up = array.GetUserPointer(index);
                if (!up) {
                    up = sq_newuserdata(SquirrelVM::GetVMPtr(), sizeof(T));
                    new(static_cast<T*>(up)) T;
                    array.SetUserPointer(index, up);
                }
                return *static_cast<T*>(up);
            }
        };
        /*
          storage of wrapped C++ functions
         */
        typedef SQInteger(*WrappedFunction)(HSQUIRRELVM, bool, int);
        typedef FlexArray<WrappedFunction> SQWrappedFuncArray;
        typedef FlexArray<SQWrappedFuncArray> SQWrappedFuncArray2 ;
        typedef FlexArray<SQWrappedFuncArray2> SQWrappedFuncArray3 ;

        struct MemberHolder {
            static SQWrappedFuncArray &funcs(int functionIndex,
                                             int paramCount) {
                static SQWrappedFuncArray3 funcs;
                return funcs.get(paramCount).get(functionIndex);
            }
        };
        struct StaticHolder {
            static SQWrappedFuncArray &funcs(int functionIndex,
                                             int paramCount) {
                static SQWrappedFuncArray3 funcs;
                return funcs.get(paramCount).get(functionIndex);
            }
        };
        struct ConstructorHolder {
            static SQWrappedFuncArray &funcs(int paramCount) {
                static SQWrappedFuncArray2 funcs;
                return funcs.get(paramCount);
            }
        };
            
        /*
          wrapper for C++ functions
         */
        template<typename Mfunc> struct MemberDispatcher {
            static inline FlexArray<Mfunc> &mfunc(void) {
                static FlexArray<Mfunc> mfunc;
                return mfunc;
            }
            static inline SQInteger
            dispatch(HSQUIRRELVM v, bool execute, int functionIndex) {
                return execute ?
                    Call(*GetInstance<TClassType, true>(v, 1),
                         mfunc().get(functionIndex), v, 2) :
                    Arg<Mfunc>::argTypeDistance(v);
            }
        };
        template<typename Sfunc> struct StaticDispatcher {
            static inline FlexArray<Sfunc> &sfunc(void) {
                static FlexArray<Sfunc> sfunc;
                return sfunc;
            }
            static inline SQInteger
            dispatch(HSQUIRRELVM v, bool execute, int functionIndex) {
                return execute ?
                    Call(sfunc().get(functionIndex), v, 2) :
                    Arg<Sfunc>::argTypeDistance(v);
            }
        };
        template<typename Cfunc> struct Constructor {
            static inline Cfunc &cfunc(void) {
                static Cfunc cfunc = 0;
                return cfunc;
            }
            static inline SQInteger
            construct(HSQUIRRELVM v, bool execute, int) {
                return execute ?
                    Call(cfunc(), v, 2) :
                    Arg<Cfunc>::argTypeDistance(v);
            }
        };

        // search and call an overloaded function on runtime
        static inline SQInteger
        call(SQWrappedFuncArray &funcs, HSQUIRRELVM v, int functionIndex = 0) {
            bool ambiguous = false;
            int imin = -1;
            int dmin = INT_MAX;
            for (int i = 0, size = funcs.size(); i < size; ++i) {
                // FIXME: to be refactored
                const int d = (**funcs.get(i))(v, false, functionIndex);
                if (d == 0) { // complete match
                    imin = i;
                    ambiguous = false;
                    goto SQPLUS_OVERLOAD_CALL_IMMEDIATE_EXECUTION;
                } else if (0 < d && d < dmin) {
                    dmin = d;
                    imin = i;
                    ambiguous = false;
                } else if (d == dmin) {
                    ambiguous = true;
                }
            }

            if (ambiguous) {
                return sq_throwerror(
                    v, _T("Call of overloaded function is ambiguous")
                    );
            } else if (imin == -1) {
                return sq_throwerror(
                    v, _T("No match for given arguments")
                    );
            }

          SQPLUS_OVERLOAD_CALL_IMMEDIATE_EXECUTION:
            // FIXME: to be refactored
            return (**funcs.get(imin))(v, true, functionIndex);
        }
        
    public:
        template<typename Mfunc> static inline void
        addMemberFunc(int functionIndex, Mfunc mfunc) {
            MemberHolder::funcs(functionIndex, Arg<Mfunc>::num()).push_back(
                &MemberDispatcher<Mfunc>::dispatch
                );
            MemberDispatcher<Mfunc>::mfunc().set(functionIndex, mfunc);
        }
        template<typename Sfunc> static inline void
        addStaticFunc(int functionIndex, Sfunc sfunc) {
            StaticHolder::funcs(functionIndex, Arg<Sfunc>::num()).push_back(
                &StaticDispatcher<Sfunc>::dispatch
                );
            StaticDispatcher<Sfunc>::sfunc().set(functionIndex, sfunc);
        }
        template<typename Cfunc> static inline void
        addConstructor(Cfunc cfunc) {
            ConstructorHolder::funcs(Arg<Cfunc>::num()).push_back(
                &Constructor<Cfunc>::construct
                );
            Constructor<Cfunc>::cfunc() = cfunc;
        }
            
        static inline SQInteger
        memberCall(int paramCount, HSQUIRRELVM v, int functionIndex) {
            return call(MemberHolder::funcs(functionIndex, paramCount),
                        v, functionIndex);
        }
        static inline SQInteger
        staticCall(int paramCount, HSQUIRRELVM v, int functionIndex) {
            return call(StaticHolder::funcs(functionIndex, paramCount),
                        v, functionIndex);
        }
        static inline SQInteger
        constructorCall(int paramCount, HSQUIRRELVM v, int) {
            return call(ConstructorHolder::funcs(paramCount), v);
        }
    }; // class SQFuncHolder


    struct FunctionNameEnumerator {
        SquirrelObject names;
        FunctionNameEnumerator(void) : names(SquirrelVM::CreateTable()) {}
        int index(const SQChar *n) {
            int i;
            SquirrelObject v = names.GetValue(n);
            if (v.IsNull()) {
                i = names.Len();
                names.SetValue(n, i);
            } else {
                i = v.ToInteger();
            }
            return i;
        }
    };

    FunctionNameEnumerator overloadedMemberNames;
    FunctionNameEnumerator overloadedStaticMemberNames;

    template<typename Func>
    class SQOverloader {
    private:
        static inline int &functionIndex(void) {
            static int index;
            return index;
        }
        
        static inline SQInteger switcher(HSQUIRRELVM v,
                                         int(*caller)(int, HSQUIRRELVM, int)) {
            return (*caller)(StackHandler(v).GetParamCount() - 1,
                             v,
                             functionIndex());
        }
        
        static inline SQInteger memberSwitcher(HSQUIRRELVM v) {
            return switcher(v, SQFuncHolder::memberCall);
        }
        static inline SQInteger staticSwitcher(HSQUIRRELVM v) {
            return switcher(v, SQFuncHolder::staticCall);
        }
        static inline SQInteger constructorSwitcher(HSQUIRRELVM v) {
            return switcher(v, SQFuncHolder::constructorCall);
        }
        
    public:
        static inline void addMemberFunc(SQClassDefBase<TClassType> *def,
                                         Func mfunc,
                                         const SQChar *name) {
            functionIndex() = def->overloadedMemberNames.index(name);
            SQFuncHolder::addMemberFunc(functionIndex(), mfunc);
            def->staticFuncVarArgs(memberSwitcher, name);
        }
        static inline void addStaticFunc(SQClassDefBase<TClassType> *def,
                                         Func sfunc,
                                         const SQChar *name) {
            functionIndex() = def->overloadedStaticMemberNames.index(name);
            SQFuncHolder::addStaticFunc(functionIndex(), sfunc);
            def->staticFuncVarArgs(staticSwitcher, name);
        }
        template<typename Cfunc>
        static inline void addConstructor(SQClassDefBase<TClassType> *def,
                                          Cfunc cfunc) {
            SQFuncHolder::addConstructor(cfunc);
            def->staticFuncVarArgs(constructorSwitcher, _T("constructor"));
        }
        static inline void addGlobalFunc(SQClassDefBase<TClassType> *def,
                                         Func gfunc,
                                         const SQChar *name) {
            functionIndex() = def->overloadedStaticMemberNames.index(name);
            SQFuncHolder::addStaticFunc(functionIndex(), gfunc);
            SquirrelVM::CreateFunctionGlobal(staticSwitcher, name, _T("*"));
        }
    };

    static inline SQRELEASEHOOK &release(void) {
        static SQRELEASEHOOK hook = ReleaseClassPtr<TClassType>::release;
        return hook;
    }

public:
    template<typename Mfunc>
    SQClassDefBase<TClassType> &overloadFunc(Mfunc mfunc, const SQChar *n) {
        SQOverloader<Mfunc>::addMemberFunc(this, mfunc, n);
        return *this;
    }
    template<typename Sfunc>
    SQClassDefBase<TClassType> &overloadStaticFunc(Sfunc sfunc,
                                                   const SQChar *n) {
        SQOverloader<Sfunc>::addStaticFunc(this, sfunc, n);
        return *this;
    }
    template<typename Cmetafunc>
    SQClassDefBase<TClassType> &overloadConstructor(void) {
        SQOverloader<Cmetafunc>::addConstructor(this, &Arg<Cmetafunc>::create);
        return *this;
    }
    template<typename Cfunc>
    SQClassDefBase<TClassType> &overloadConstructor(Cfunc cfunc) {
        SQOverloader<Cfunc>::addConstructor(this, cfunc);
        return *this;
    }
    template<typename Gfunc>
    SQClassDefBase<TClassType> &overloadGlobalFunc(Gfunc gfunc,
                                                   const SQChar *n) {
        SQOverloader<Gfunc>::addGlobalFunc(this, gfunc, n);
        return *this;
    }

    SQClassDefBase<TClassType> &releaseHook(SQRELEASEHOOK releaseHook) {
        release() = releaseHook;
        return *this;
    }
    static inline SQRELEASEHOOK &getReleaseHook(void) {
        return release();
    }


#endif // SQPLUS_OVERLOAD_IMPLEMENTATION



#ifdef SQPLUS_OVERLOAD_FUNCTIONS
#undef SQPLUS_OVERLOAD_FUNCTIONS

struct GlobalFuncOverloader {};
static inline SQClassDefBase<GlobalFuncOverloader> &globalFuncOverloader(void)
{
    static SQClassDefBase<GlobalFuncOverloader> fo(_T("GlobalFuncOverloader"));
    return fo;
}

template<typename Gfunc> void
OverloadGlobal(Gfunc gfunc, const SQChar *n)
{
    globalFuncOverloader().overloadGlobalFunc(gfunc, n);
}

template<typename TYPE> struct CheckInstance {
    template<typename T> struct unref {typedef T type;};
    template<typename T> struct unref<T&> {typedef T type;};
    template<typename T> struct unref<T*> {typedef T type;};
    template<typename T> struct unref<const T&> {typedef T type;};
    template<typename T> struct unref<const T*> {typedef T type;};
    
    /*
      d = -1 : not in hierarchy
      d = 0  : same
      d > 0  : similar (o is d-th subclass of TYPE)
    */
    static inline int distance(HSQUIRRELVM v, int index) {
        HSQOBJECT o;
        sq_resetobject(&o);
        sq_getstackobj(v, index, &o);
    
        const int top = sq_gettop(v);
        sq_pushroottable(v);

        // Check plain object type
        int d = -1;
        if (Match(TypeWrapper<TYPE>(), v, index)) {
            d = 0;
            
            // Check instance type hierarchy
            if (sq_type(o) == OT_INSTANCE) {
                SQUserPointer dsttype =
                    ClassType<typename unref<TYPE>::type>::type();
            
                SQUserPointer argtype;
                for (sq_getclass(v, index);
                     sq_gettypetag(v, -1, &argtype) == SQ_OK;
                     sq_getbase(v, -1)) {
                    if (argtype == dsttype) {
                        goto SQPLUS_OVERLOAD_DISTANCE_IMMEDIATE_RETURN;
                    }
                    ++d;
                }
                d = -1;         // no matching type found
            }
        }
      SQPLUS_OVERLOAD_DISTANCE_IMMEDIATE_RETURN:
        sq_settop(v, top);
        return d;
    }
};


template<typename T, typename R>
struct Arg<R(T::*)(void)> {
    static inline int num(void) {return 0;}
    static inline int argTypeDistance(HSQUIRRELVM) {
        return 0;
    }
};

template<typename T, typename R, typename A1>
struct Arg<R(T::*)(A1)> {
    static inline int num(void) {return 1;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        return Arg<R(*)(A1)>::argTypeDistance(v);
    }
};

template<typename T, typename R, typename A1, typename A2>
struct Arg<R(T::*)(A1, A2)> {
    static inline int num(void) {return 2;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        return Arg<R(*)(A1, A2)>::argTypeDistance(v);
    }
};

template<typename T, typename R, typename A1, typename A2, typename A3>
struct Arg<R(T::*)(A1, A2, A3)> {
    static inline int num(void) {return 3;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        return Arg<R(*)(A1, A2, A3)>::argTypeDistance(v);
    }
};

template<typename T, typename R, typename A1, typename A2, typename A3, typename A4>
struct Arg<R(T::*)(A1, A2, A3, A4)> {
    static inline int num(void) {return 4;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        return Arg<R(*)(A1, A2, A3, A4)>::argTypeDistance(v);
    }
};

template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
struct Arg<R(T::*)(A1, A2, A3, A4, A5)> {
    static inline int num(void) {return 5;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        return Arg<R(*)(A1, A2, A3, A4, A5)>::argTypeDistance(v);
    }
};

template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct Arg<R(T::*)(A1, A2, A3, A4, A5, A6)> {
    static inline int num(void) {return 6;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        return Arg<R(*)(A1, A2, A3, A4, A5, A6)>::argTypeDistance(v);
    }
};

template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct Arg<R(T::*)(A1, A2, A3, A4, A5, A6, A7)> {
    static inline int num(void) {return 7;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        return Arg<R(*)(A1, A2, A3, A4, A5, A6, A7)>::argTypeDistance(v);
    }
};

static inline int classAllocationError(HSQUIRRELVM v) {
    return sq_throwerror(v, _T("Failed to allocate memory"));
}

template<typename R>
struct Arg<R(*)(void)> {
    static inline int num(void) {return 0;}
    static inline int argTypeDistance(HSQUIRRELVM) {return 0;}
    static inline int create(void) {
        HSQUIRRELVM v = SquirrelVM::GetVMPtr();
        R *r = static_cast<R*>(sq_malloc(sizeof(R)));
        return r ?
            PostConstruct<R>(v, new(r) R,
                             SQClassDefBase<R>::getReleaseHook()) :
            classAllocationError(v);
    }
};

template<typename R, typename A1>
struct Arg<R(*)(A1)> {
    static inline int num(void) {return 1;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        int s, r;
        r = 0;
        s = CheckInstance<A1>::distance(v, 2); if (s < 0) {return -1;} r += s;
        return r;
    }
    static inline int create(A1 a1) {
        HSQUIRRELVM v = SquirrelVM::GetVMPtr();
        R *r = static_cast<R*>(sq_malloc(sizeof(R)));
        return r ?
            PostConstruct<R>(v, new(r) R(a1),
                             SQClassDefBase<R>::getReleaseHook()) :
            classAllocationError(v);
    }
};

template<typename R, typename A1, typename A2>
struct Arg<R(*)(A1, A2)> {
    static inline int num(void) {return 2;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        int s, r;
        r = 0;
        s = CheckInstance<A1>::distance(v, 2); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A2>::distance(v, 3); if (s < 0) {return -1;} r += s;
        return r;
    }
    static inline int create(A1 a1, A2 a2) {
        HSQUIRRELVM v = SquirrelVM::GetVMPtr();
        R *r = static_cast<R*>(sq_malloc(sizeof(R)));
        return r ?
            PostConstruct<R>(v, new(r) R(a1, a2),
                             SQClassDefBase<R>::getReleaseHook()) :
            classAllocationError(v);
    }
};

template<typename R, typename A1, typename A2, typename A3>
struct Arg<R(*)(A1, A2, A3)> {
    static inline int num(void) {return 3;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        int s, r;
        r = 0;
        s = CheckInstance<A1>::distance(v, 2); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A2>::distance(v, 3); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A3>::distance(v, 4); if (s < 0) {return -1;} r += s;
        return r;
    }
    static inline int create(A1 a1, A2 a2, A3 a3) {
        HSQUIRRELVM v = SquirrelVM::GetVMPtr();
        R *r = static_cast<R*>(sq_malloc(sizeof(R)));
        return r ?
            PostConstruct<R>(v, new(r) R(a1, a2, a3),
                             SQClassDefBase<R>::getReleaseHook()) :
            classAllocationError(v);
    }
};

template<typename R, typename A1, typename A2, typename A3, typename A4>
struct Arg<R(*)(A1, A2, A3, A4)> {
    static inline int num(void) {return 4;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        int s, r;
        r = 0;
        s = CheckInstance<A1>::distance(v, 2); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A2>::distance(v, 3); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A3>::distance(v, 4); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A4>::distance(v, 5); if (s < 0) {return -1;} r += s;
        return r;
    }
    static inline int create(A1 a1, A2 a2, A3 a3, A4 a4) {
        HSQUIRRELVM v = SquirrelVM::GetVMPtr();
        R *r = static_cast<R*>(sq_malloc(sizeof(R)));
        return r ?
            PostConstruct<R>(v, new(r) R(a1, a2, a3, a4),
                             SQClassDefBase<R>::getReleaseHook()) :
            classAllocationError(v);
    }
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
struct Arg<R(*)(A1, A2, A3, A4, A5)> {
    static inline int num(void) {return 5;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        int s, r;
        r = 0;
        s = CheckInstance<A1>::distance(v, 2); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A2>::distance(v, 3); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A3>::distance(v, 4); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A4>::distance(v, 5); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A5>::distance(v, 6); if (s < 0) {return -1;} r += s;
        return r;
    }
    static inline int create(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
        HSQUIRRELVM v = SquirrelVM::GetVMPtr();
        R *r = static_cast<R*>(sq_malloc(sizeof(R)));
        return r ?
            PostConstruct<R>(v, new(r) R(a1, a2, a3, a4, a5),
                             SQClassDefBase<R>::getReleaseHook()) :
            classAllocationError(v);
    }
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct Arg<R(*)(A1, A2, A3, A4, A5, A6)> {
    static inline int num(void) {return 6;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        int s, r;
        r = 0;
        s = CheckInstance<A1>::distance(v, 2); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A2>::distance(v, 3); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A3>::distance(v, 4); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A4>::distance(v, 5); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A5>::distance(v, 6); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A6>::distance(v, 7); if (s < 0) {return -1;} r += s;
        return r;
    }
    static inline int create(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {
        HSQUIRRELVM v = SquirrelVM::GetVMPtr();
        R *r = static_cast<R*>(sq_malloc(sizeof(R)));
        return r ?
            PostConstruct<R>(v, new(r) R(a1, a2, a3, a4, a5, a6),
                             SQClassDefBase<R>::getReleaseHook()) :
            classAllocationError(v);
    }
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct Arg<R(*)(A1, A2, A3, A4, A5, A6, A7)> {
    static inline int num(void) {return 7;}
    static inline int argTypeDistance(HSQUIRRELVM v) {
        int s, r;
        r = 0;
        s = CheckInstance<A1>::distance(v, 2); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A2>::distance(v, 3); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A3>::distance(v, 4); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A4>::distance(v, 5); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A5>::distance(v, 6); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A6>::distance(v, 7); if (s < 0) {return -1;} r += s;
        s = CheckInstance<A7>::distance(v, 8); if (s < 0) {return -1;} r += s;
        return r;
    }
    static inline int create(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) {
        HSQUIRRELVM v = SquirrelVM::GetVMPtr();
        R *r = static_cast<R*>(sq_malloc(sizeof(R)));
        return r ?
            PostConstruct<R>(v, new(r) R(a1, a2, a3, a4, a5, a6, a7),
                             SQClassDefBase<R>::getReleaseHook()) :
            classAllocationError(v);
    }
};
#endif // SQPLUS_OVERLOAD_FUNCTIONS

// SqPlusOverload.h

// Local Variables: 
// mode: c++
// End:

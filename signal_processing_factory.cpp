template<typename T>
struct TT {
};

template<typename T>
struct abstract_creator {
    virtual unique_ptr<T> doCreate(TT<T> &&) = 0;
};

template<typename... Ts>
struct signal_processing_factory : public abstract_creator<Ts>... {
    
    template<class U> unique_ptr<U> create() {
        abstract_creator<U> &creator = *this;
        return creator.doCreate(TT<U>());
    }
    virtual ~signal_processing_factory() {}
};

template<typename AbstractFactory, typename Abstract, typename Concrete>
struct concrete_creator : virtual public AbstractFactory {
    unique_ptr<Abstract> doCreate(TT<Abstract> &&) override {
        return make_unique<Concrete>();
    }
};

template<typename AbstractFactory, typename... ConcreteTypes>
struct concrete_signal_processing_factory;

template<typename... AbstractTypes, typename... ConcreteTypes>
struct concrete_signal_processing_factory
<signal_processing_factory<AbstractTypes...>, ConcreteTypes...>
: public concrete_creator<signal_processing_factory<AbstractTypes...>,
AbstractTypes, ConcreteTypes>... {
};
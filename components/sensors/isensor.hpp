class ISensor {
public:
    virtual ~ISensor() = default;

    virtual void init();
    virtual void reset();
};
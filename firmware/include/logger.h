namespace Logger
{
    void init();
    void start(uint8_t cadenceSeconds);
    void stop();
    void clear();
    bool isLogging();
    bool isEmpty();
    void render();

    uint16_t maxSamples();

    bool fetch(uint16_t index, uint32_t* timestamp, int32_t* reading);
    bool update(int32_t reading);
}




class DocStats {
    unsigned int chars_, nonBlankChars_, words_, images_;
    // ...

public:
    void AddChars(unsigned int charsToAdd) {
        chars_ += charsToAdd;
    }
    void AddWords(unsigned int charsToAdd) {
        words_ += charsToAdd;
    }
    void AddImages(unsigned int charsToAdd) {
        words_ += charsToAdd;
    }

    void Display();
};

class DocElement {
    virtual void UpdateStats(DocStats& statistics) = 0;
};

class Paragraph : public DocElement {
public:
    void UpdateStats(DocStats& statistics) {
        statistics.AddChars(22); // would count number of chars
        statistics.AddWords(3); // would count number of words
    }
};


class RasterBitmap : public DocElement {
public:
    void UpdateStats(DocStats& statistics) {
        statistics.AddImages(1);
    }
};


int main() {

}

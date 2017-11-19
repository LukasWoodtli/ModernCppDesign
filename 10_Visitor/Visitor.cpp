#include <vector>
#include <iostream>

class DocElementVisitor;

class DocElement {
public:
    virtual void Accept(DocElementVisitor&) = 0;
};

class Paragraph;
class RasterBitmap;

class DocElementVisitor {
public:
    virtual void Visit(Paragraph&) = 0;
    virtual void Visit(RasterBitmap&) = 0;
    // ...
};

class Paragraph : public DocElement {
    unsigned int fontSize_;
public:
    unsigned int NumChars() {
        return 3; // would count chars here
    }
    unsigned int NumWords() {
        return 5; // woud count words here
    }
    virtual void Accept(DocElementVisitor& v) {
        v.Visit(*this);
    }
    void SetFontSize(unsigned int fontSize) {
        fontSize_ = fontSize;
    }
    unsigned int GetFontSize() const {
        return fontSize_;
    }
};


class RasterBitmap : public DocElement {
public:
    virtual void Accept(DocElementVisitor& v) {
        v.Visit(*this);
    }
};


class DocStats : public DocElementVisitor {
    unsigned int chars_, words_, images_;
public:
    virtual void Visit(Paragraph& par) {
        chars_ += par.NumChars();
        words_ += par.NumWords();
    }

    virtual void Visit(RasterBitmap&) {
        ++images_;
    }

    void Display() {
        std::cout << "Stats:\n";
        std::cout << chars_ << "\n";
        std::cout << words_ << "\n";
        std::cout << images_ << "\n\n";
    }
};

class IncrementFontSize : public DocElementVisitor {
public:
    virtual void Visit(Paragraph& par) {
        par.SetFontSize(par.GetFontSize() + 1);
    }

    virtual void Visit(RasterBitmap&) {
        // nothing to do
    }
};

class Document {
std::vector<DocElement> docElements_;

    void DisplayStatistics() {
        DocStats statistics;
        std::vector<DocElement>::iterator it;
        for (it = docElements_.begin(); it != docElements_.end(); ++it) {
            it->Accept(statistics);
        }
        statistics.Display();
    }
};


int main() {

}

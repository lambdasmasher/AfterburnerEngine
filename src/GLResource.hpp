#ifndef GLRESOURCE
#define GLRESOURCE

#include <vector>

class GLResource {
public:
    GLResource() { resources.push_back(this); }
    virtual ~GLResource() {};
    static void cleanup() {
        for (GLResource *resource : resources)
            delete resource;
    };
private:
    static inline std::vector<GLResource*> resources;
};

#endif /* GLRESOURCE */

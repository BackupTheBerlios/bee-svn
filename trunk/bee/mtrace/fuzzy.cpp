#include <map>
#include <string>

using std::string;
using std::map;


#define INT_PARAM 1

class Parameter {
        public:
                Parameter( string name, int type): _name(name), _type(type) {};

                ~Parameter();
                int type(){ return _type;} ;
                string name() { return _name;} ;
        private:
                int _type;
                string _name;
};

class Method {
        public:
                Method(){};
                Method(char* name) :_name(name){};
                ~Method(){};
                void show()
                {
                        for(int i=0; i<params.size(); ++i)
                        {
                                printf("c\n");
                        }
                }

                bool addParam(string name, int type)
                {
                        params[name] = type ;
                }
                void fuzz()
                {
                }

                void fuzz(string param)
                {
                        printf("fuzzing: %s\n", param.c_str());
                }
        private:
                map<string, int> params;
                string _name;
};

int main()
{
        Method* m = new Method("Blend");
        m->addParam("color", INT_PARAM);
        m->addParam("degree", INT_PARAM);
        printf("Name Type\n");
        m->fuzz("color");
        m->show();
        return 0;
}

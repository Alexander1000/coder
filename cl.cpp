using namespace std;

class CommandLine
{
    public:
        CommandLine (int argc, char* argv[])
        {
            this->argc = argc;
            this->argv = argv;
        }

        bool is (const char *param)
        {
            return this->is((char*) param);
        }

        /**
         * Check paramter exists
         */
        bool is (char *param)
        {
            bool isEqual = false;
            string str1(param);

            for (int i = 0; i < this->argc; ++i) {
                string str2(this->argv[i]);

                if (str1.compare(str2) == 0) {
                    isEqual = true;
                    break;
                }
            }

            return isEqual;
        }

        char* get (const char* param)
        {
            return this->get((char*) param);
        }

        char* get (char* param)
        {
            int pos = this->getPosition(param);

            if (pos > 0 && pos + 1 < this->argc) {
                return this->argv[pos + 1];
            }

            return (char*) 0x00;
        }
    protected:
        int argc;
        char** argv;

        int getPosition(char* param)
        {
            int pos = -1;
            string str1(param);

            for (int i = 0; i < this->argc; ++i) {
                string str2(this->argv[i]);

                if (str1.compare(str2) == 0) {
                    pos = i;
                    break;
                }
            }

            return pos;
        }
};

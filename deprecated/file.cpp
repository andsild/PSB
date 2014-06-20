
string get_path()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    string sCwd = string(cwd);

    size_t project_index = sCwd.find(PROJECT_DIR);
    if (project_index!=string::npos)
    {
        size_t slash_index = sCwd.find_first_of("/", project_index);
        if (slash_index!=string::npos)
            return sCwd.substr(0, slash_index + 1);  //include slash
    }
    else {
        return string("./");
    }
}



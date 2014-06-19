
void readSingleImage(vector<iterative_function> vIf)
{
    // CImg<unsigned char> image("./media/109201.jpg"); //example
    char cImageName[] = "./small_media/104000.jpg";
    string sDest = get_path() + string(cImageName);
    // CImg<unsigned char> image(sDest.c_str()); //example
    CImg<double> image ;
    try{
        image = getImage(sDest.c_str());
    }
    catch(CImgIOException cioe) {
        cout << cioe.what() << endl;
        exit(EXIT_FAILURE);
    }
    // CImgList<double> cil = readImage(image, cImageName, vIf);
}


double findError(const d1 origData, const d1 newData, int iLength)
{
    double dError = 0;
    for(std::vector<int>::size_type iPos = 0; iPos != origData.size(); iPos++)
    {
        dError += calculateError(origData[iPos],
                                 newData[iPos]);
    }
    return dError /= iLength;

}


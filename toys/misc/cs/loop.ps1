

# https://ss64.com/ps/syntax-hash-tables.html

# https://docs.microsoft.com/en-us/powershell/scripting/learn/deep-dives/everything-about-arrays?view=powershell-7.1

[int[]] $myArray = @()

for ($i=0; $i -lt 4; $i++) {
    $myArray += $i
    Write-Host "i = " + $myArray[$i] -ForegroundColor Magenta    
}

Write-Host "length = " + $myArray.length -ForegroundColor Blue    

for ($i=0; $i -lt $myArray.length; $i++) {
    Write-Host "i = " + $myArray[$i] -ForegroundColor Yellow    
}

Write-Host "iterate"
foreach($item in $myArray)
{
    Write-Output $item
}

# https://docs.microsoft.com/en-us/powershell/scripting/learn/deep-dives/everything-about-hashtable?view=powershell-7.1
#
# Hash table
$ageList = @{}
$key = 'Kevin'
$value = 36
$ageList[$key] = $value

$ageList['Claire'] = 9
$ageList['Taylor'] = 7
$ageList['Mac'] = 8


foreach($key in $ageList.keys)
{
    $message = '{0} is {1} years old' -f $key, $ageList[$key]
    Write-Output $message
}


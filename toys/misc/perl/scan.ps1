# https://docs.microsoft.com/en-us/powershell/scripting/samples/sample-scripts-for-administration?view=powershell-7.2
# https://www.improvescripting.com/powershell-function-begin-process-end-blocks-explained-with-examples/
# https://stackoverflow.com/questions/885349/how-do-i-write-a-powershell-script-that-accepts-pipeline-input

# cat .\app.log | .\scan.ps1
# 1..20 | .\scan.ps1
#Write-Host "hello, world!"
Function dude($line) {
    begin { # runs once at each invocation
        $a = 42
    }
    process {
        if ($line -match '.*iter (\d{2})') {
        #if ($line -match '.*iter(.*)') {
        #if ($line -match '.*net stop.*(Sur.*)') {
            $m = $Matches.Item(1)
            Write-Host "iter:  $m"
        } else {
            Write-Host "iter not found $line"
        }
    }
    end {
        return $a
    }
}

foreach ($i in $input) {
    #$runs = (0,1,2,3,$i)
    $n = dude($i)
    echo "homi: $n"
}



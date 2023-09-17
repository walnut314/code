Start-Transcript -Path .\testlog.txt
Get-ChildItem –Path ".\" –Recurse |
Foreach-Object {
    if ($_.FullName -match '.log$') { # only files ending in .log
        Write-Host $_.FullName
        -split (Get-Content $_.FullName -Raw) |
            Group-Object -NoElement |
            Sort-Object Count -Descending |
            Select-Object @{ N='Word'; E='Name' }, @{ N='Frequency'; E='Count' }
    }

}
Stop-Transcript

#!\usr\bin\env powershell

# cat .\app.log | .\scan.ps1
# 1..20 | .\scan.ps1

Set-Alias -Name printf -Value Write-Host

function dude($line) {
    begin { # runs once at each invocation
        $a = 42
    }
    process {
        if ($line -match '.*iter (\d{2})') {
        #if ($line -match '.*iter(.*)') {
        #if ($line -match '.*net stop.*(Sur.*)') {
            $m = $matches.Item(1);
            printf("iter:  $m");
        } else {
            printf("iter not found $line");
        }
    }
    end {
        return $a
    }
}

function stuff($msg) {
    printf("$msg");

    $hash = @{} # empty hash
    $hash = @{ Number = 1; Shape = "Square"; Color = "Blue"}
    $hash.Add("Dude", 42)
    $hash.Add("Sup", 17)
    $keys = $hash.keys;
    printf("dude: $keys");
    foreach ($k in $hash.keys) { $v = $hash[$k]; printf("$k => $v"); }

    # arrays (lists)
    $list = @(); # empty list
    $list = @('Zero', 'One', 'Two', 'Three');
    $list = $list + 'Four'; # adding to list
    $c = $list.count;
    $item = $list[$c-1]
    printf("list count: $c => $item");
    foreach ($i in $list) { printf("item: $i"); }
}

foreach ($i in $input) {
    #$runs = (0,1,2,3,$i)
    $n = dude($i);
    printf("homi: $n");
}
stuff("dude stuff");



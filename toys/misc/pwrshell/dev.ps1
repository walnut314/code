class Device {
    [string]$Brand
    [string]$Model
    [string]$VendorSku
    [string]ToString(){
        return ("{0}|{1}|{2}" -f $this.Brand, $this.Model, $this.VendorSku)
    }
}

class Rack {
    [int]$Slots = 8
    [string]$Brand
    [string]$Model
    [string]$VendorSku
    [string]$AssetId
    [Device[]]$Devices = [Device[]]::new($this.Slots)
    [void] AddDevice([Device]$dev, [int]$slot){
        $this.Devices[$slot] = $dev
    }
    [void]RemoveDevice([int]$slot){
        $this.Devices[$slot] = $null
    }
    [int[]] GetAvailableSlots(){
        [int]$i = 0
        return @($this.Devices.foreach{ if($_ -eq $null){$i}; $i++})
    }
    [void] DumpSlots(){
        for ($i=0; $i -lt $this.Slots; $i++){ #$dev in $this.Devices){
            $myString = "'$($i)' is '$($this.Devices[$i])'"
#https://hodgkins.io/the-ultimate-guide-to-powershell-string-formatting                
            Write-Host $myString # Write-Output
        }
    }
}


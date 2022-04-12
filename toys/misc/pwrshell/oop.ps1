# include header
. .\dev.ps1

$rack = [Rack]::new()
$surface = [Device]::new()
$surface.Brand = "Microsoft"
$surface.Model = "Surface Pro 4"
$surface.VendorSku = "5072641000"
$rack.AddDevice($surface, 2)
$rack.AddDevice($surface, 7)
#$rack
$rack.DumpSlots()
$rack.GetAvailableSlots()


#!/usr/bin/env python3

import win32com.client as win32

outlook = win32.Dispatch('outlook.application')
mail = outlook.CreateItem(0)
mail.To = 'v-babart@microsoft.com'
mail.Subject = 'dude wusup'
mail.Body = 'owzit'
mail.HTMLBody = '<h2>hooda homi</h2>' #this field is optional

# To attach a file to the email (optional):
#attachment  = "Path to the attachment"
#mail.Attachments.Add(attachment)

mail.Send()

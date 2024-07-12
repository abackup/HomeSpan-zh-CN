<!--  原文时间：2023.4.6，翻译时间：2024.5.6，校对时间：2024.7.12   -->

# 将配对数据从一台设备克隆到另一台设备

### HomeSpan 配对数据

尽管两个不同的 ESP32 设备可能运行完全相同的草图，但它们仍然是不同的。这是因为每个 HomeSpan 配件都有唯一的 17 字符设备 ID、唯一的 32 字节长期公钥 (LTPK) 和唯一的 64 字节长期密钥 (LTSK)。当 HomeSpan 首次在新设备上运行时，它会在设备的非易失性存储 (NVS) 内存中查找这些数据。如果找到，则加载数据以供使用。如果未找到，HomeSpan 会生成一组新的随机密钥，并将该数据保存在 NVS 中。数据被永久存储，但可以通过在 CLI 中键入 "H" 来删除，这会导致 HomeSpan 在下次重新启动时生成一组新的随机密钥。

当 HomeSpan 最初与 HomeKit 配对时，一个或多个 HomeKit 控制权的 36 字符设备 ID 和 32 字节 LTPK 会安全地传输到 HomeSpan 配件。然后，这些密钥将保存在设备的 NVS 中以永久保留（但可以通过 "H" 命令擦除）。

配件设备 ID、LTPK 和 LTSK 以及每个配对控制权的设备 ID 和 LTPK 统称为设备的*配对数据*。你可以通过在 CLI 中键入 "S" 来查看任何 HomeSpan 配件的配对数据（LTSK 除外）。这是一个例子：

```
*** HomeSpan Status ***

IP Address:        192.168.1.11

Accessory ID:      77:D2:F6:99:CE:65                               LTPK: 346A544A876B124E50F9E3CC276A29D23E8B5DD0590138AA59C833A0D2096E37
Paired Controller: A487DE69-81C3-B5ED-8762-C3B9A987F967   (admin)  LTPK: EE12A678DD56C4E9C0D935A341B8E6C6C098A6B3E6D4C5F5F914A54C9E85BA76
Paired Controller: 449AD09E-109D-3EB5-25B4-8A04E5C57D65   (admin)  LTPK: 34A6B57DE881A75B647D2C9C68E76745A3B466577D19E4C78A67A68C4ED959B8

Connection #0 192.168.1.29 on Socket 3/16  ID=A487DE69-81C3-B5ED-8762-C3B9A987F967   (admin)
Connection #1 (unconnected)
Connection #2 (unconnected)
Connection #3 (unconnected)
Connection #4 (unconnected)
Connection #5 (unconnected)
Connection #6 (unconnected)
Connection #7 (unconnected)
Connection #8 (unconnected)
Connection #9 (unconnected)
Connection #10 (unconnected)
Connection #11 (unconnected)

*** End Status ***
```

### 克隆 HomeSpan 设备

由于每个设备都有一组唯一的配对数据，因此不可能简单地将已与 HomeKit 配对的设备替换另一台尚未配对的设备 - 设备 ID 会有所不同，并且需要单独对第二个设备进行单独配对。

如果你需要更换设备（可能是因为设备出现故障或损坏）并且你已从“家庭”应用中为设备创建了许多自定义自动化和场景，这可能会出现问题。你用来替换旧设备的新设备将被 HomeKit 视为全新的附件，并且不会与与旧设备相关的任何自动化或场景连接。事实上，如果你取消与旧设备的配对，特定于该设备的自动化和场景将会丢失。

要解决此问题，你需要能够用新设备替换损坏的设备，但*无需*取消旧设备的配对或重新配对新设备。这要求新设备不是使用一组新的随机生成的设备 ID、LTPK 和 LTSK 进行初始化，而是使用与旧设备*相同*的配对数据。

幸运的是，HomeSpan 提供了一种将配对数据从一个设备“克隆”到另一个设备的方法。这意味着你可以将损坏的设备更换为新设备，而 HomeKit 不知道其中的差异（当然前提是它运行相同的草图）。事实上，你甚至可以将 ESP32 更换为 ESP32-S2 或 ESP32-C3。只要草图相同，一旦克隆了配对数据，设备就可以有效地热替换。

克隆 HomeSpan 的配对数据分为两步。首先，将配对数据从一个设备输出到串口监视器，然后将此数据复制并粘贴到第二个设备的串口监视器中。当然，如果第一个设备完全损坏，你将无法输出其配对数据。如果你在 HomeKit 中创建了大量自动化功能，你可能希望从每个设备输出配对数据并将其保存在纯文本文件中，以供将来需要更换任何设备时使用。

#### 步骤 1：在第一个设备的串口监视器 CLI 中键入 "P" 以输出其配对数据

与 "S" 命令不同，"P" 命令将所有配对数据压缩为 *base-64* 块，以便更轻松地复制和粘贴，如下所示：

```
*** 用于克隆另一台设备的配对数据

*** Pairing Data used for Cloning another Device

Accessory data:  ZzbH11I8uNx47Y3Bapq3axQfY5uPOrDfC8D2Q6ke2NwWqat/IGa/6ll8xyY8AShMYO2q6h8gZr/qWXzHJjwBKExg7arqFnNsfXUjy43HgNzc6RDI6RjY6OTk6Q0U6NjUb7mHwbmWzrEWca+5frayfmp=
Controller data: YaNJH5JYDAQE4NjI0NTAwNy02Mi1FRUY4ODNENTA2NjdDvTRGLTRBRDEtQjkwRXFM1On32PKvumS+0YgVMaEo53X/TYNzg==
Controller data: MEUwLTREMEUtODk3Ni0yMjBDREQ2RDUxMjjmah3s+Je0GkmAQE0NDQ1NUE2Ni1ExIUkujzeyWfCCRWol/xecsVkjAIYDRQ==

*** End Pairing Data
```

第一行完整编码 HomeSpan 配件的配对数据。后两行对 HomeKit 用于控制 HomeSpan 设备的两个控制权的配对数据进行编码。请注意，你的系统可能只有一个控制权，也可能有两个以上。控制权的数量取决于你的 HomeKit 网络、其配置方式、你拥有的设备（Apple TV、HomePod 等）以及你运行的 iOS 版本。

从 CLI 中按原样复制此数据并将其保存在文本文件中。确保不要丢失任何尾随等号，因为它们是 base&#8209;64 数据的一部分！

接下来，关闭第一个设备的电源，或者至少将其从 WiFi 网络中移除，以避免在同一网络上运行且具有相同配对数据的两个设备的潜在重复（如果发生这种情况，HomeKit 可能无法正常工作）。如果第二个设备未插入，请立即插入并打开其串口监视器。

#### 步骤 2：在第二个设备的串口监视器 CLI 中键入 "C" ，以输入你刚刚从第一个设备保存的配对数据

HomeSpan 将首先询问你配件配对数据。将此数据（这是上面步骤 1 中输出的第一组 Base-64 数据）直接复制并粘贴到串口监视器输入窗口中，然后按回车键。如果你正确复制了数据，它将被接受，并且 HomeSpan 将显示数据中编码的设备 ID（它不会显示 LTPK 和 LTSK 数据）。设备 ID 应与原始设备的 ID 匹配。

如果你错误地复制或粘贴了数据，HomeSpan 将通知你存在问题，取消该过程，然后重新启动而不进行任何更改。你还可以通过在键入 "P" 后单击“回车”来取消该过程，*无需*输入任何数据（这不会导致重新启动，因为没有更改数据）。

接受配件数据后，HomeSpan 将请求控制权数据。将保存的文本文件中的控制权之一的 Base-64 数据直接复制并粘贴到串口监视器输入窗口中，然后按回车键。和以前一样，如果复制和粘贴正确，HomeSpan 将接受数据并显示控制权的设备 ID。如果你复制和粘贴不正确，HomeSpan 将通知你存在错误，取消该过程，然后重新启动而不进行任何更改。

假设第一个控制权的数据已被接受，HomeSpan 将要求你对你可能拥有的任何其他控制权重复该过程。继续重复复制和粘贴每个控制权的配对数据的过程。输入所有控制权的配对数据后，当询问下一个控制权时，只需按回车键，无需输入任何数据。一个空响应会告知 HomeSpan 你已完成添加控制权数据。

最后，HomeSpan 会要求你确认保存新数据。输入 "y" 确认（是）或输入 "n" 取消（否）。如果你输入 "n"，HomeSpan 将重新启动而不保存任何更改。

如果你键入 "y"，HomeSpan 会将所有新的配对数据保存在设备的 NVS 中并重新启动。重新启动后，第二个设备将是第一个设备的完美克隆，HomeKit 应该将其识别为原始设备。你无需重新配对设备或对“家庭”应用进行任何其他更改。
  
❗注意：请勿在同一 HomeKit 网络上使用相同的配对数据运行两个设备。如果你想通过将工作设备克隆到第二个设备上进行实验，请确保在将数据克隆到第二个设备上之前拔下第一个设备的插头。完成实验后，在其中一台设备的 CLI 中输入 "H"，这样克隆的配对数据将被删除并重新生成为唯一的数据，从而允许你同时插入两个设备而不会发生冲突。

---

[↩️](../README.md#resources) 返回欢迎页面

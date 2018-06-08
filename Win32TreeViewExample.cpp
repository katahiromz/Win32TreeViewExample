#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>

LPARAM TV_GetParam(HWND hwndTV, HTREEITEM hItem = NULL)
{
    if (!hItem)
        hItem = TreeView_GetSelection(hwndTV);

    TV_ITEM item;
    ZeroMemory(&item, sizeof(item));
    item.mask = TVIF_PARAM;
    item.hItem = hItem;
    TreeView_GetItem(hwndTV, &item);

    return item.lParam;
}

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HWND hwndTV = GetDlgItem(hwnd, ctl1);

    INT nValue = 1;
    TV_INSERTSTRUCT insert;
    ZeroMemory(&insert, sizeof(insert));
    insert.hParent = TVI_ROOT;
    insert.hInsertAfter = TVI_FIRST;
    insert.item.mask = TVIF_TEXT | TVIF_PARAM;
    insert.item.lParam = nValue;

    TCHAR szText[] = TEXT("Root");
    insert.item.pszText = szText;
    TreeView_InsertItem(hwndTV, &insert);

    printf("#%d was inserted\n", nValue);
    fflush(stdout);

    ++nValue;
    wsprintf(szText, TEXT("Item #%d"), nValue);
    SetDlgItemText(hwnd, edt1, szText);
    SetDlgItemInt(hwnd, edt2, nValue, TRUE);

    return TRUE;
}

void OnPsh1(HWND hwnd)
{
    HWND hwndTV = GetDlgItem(hwnd, ctl1);
    HTREEITEM hItem = TreeView_GetSelection(hwndTV);

    TCHAR szText[64];
    GetDlgItemText(hwnd, edt1, szText, 64);
    INT nValue = GetDlgItemInt(hwnd, edt2, NULL, TRUE);

    TV_INSERTSTRUCT insert;
    ZeroMemory(&insert, sizeof(insert));
    insert.hParent = hItem;
    insert.hInsertAfter = TVI_LAST;
    insert.item.mask = TVIF_TEXT | TVIF_PARAM;
    insert.item.lParam = nValue;
    insert.item.pszText = szText;
    HTREEITEM hNewItem = TreeView_InsertItem(hwndTV, &insert);

    if (hNewItem)
    {
        printf("#%d was inserted\n", nValue);
        fflush(stdout);

        ++nValue;
        wsprintf(szText, TEXT("Item #%d"), nValue);
        SetDlgItemText(hwnd, edt1, szText);
        SetDlgItemInt(hwnd, edt2, nValue, TRUE);

        if (1)
        {
            TreeView_Expand(hwndTV, hItem, TVE_EXPAND);
            TreeView_EnsureVisible(hwndTV, hNewItem);
        }
    }

    InvalidateRect(hwndTV, NULL, TRUE);
}

void OnPsh2(HWND hwnd)
{
    HWND hwndTV = GetDlgItem(hwnd, ctl1);

    HTREEITEM hItem = TreeView_GetSelection(hwndTV);
    TreeView_DeleteItem(hwndTV, hItem);
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
        if (codeNotify == BN_CLICKED)
        {
            EndDialog(hwnd, IDOK);
        }
        break;
    //case IDCANCEL:
    //    EndDialog(hwnd, IDCANCEL);
    //    break;
    case psh1:
        if (codeNotify == BN_CLICKED)
            OnPsh1(hwnd);
        break;
    case psh2:
        if (codeNotify == BN_CLICKED)
            OnPsh2(hwnd);
        break;
    }
}

LRESULT OnNotify(HWND hwnd, int idFrom, LPNMHDR pnmhdr)
{
    HWND hwndTV = GetDlgItem(hwnd, ctl1);
    LPARAM lParam;
    NM_TREEVIEW *pTreeView;
    TV_DISPINFO *pDispInfo;

    switch (pnmhdr->code)
    {
    case TVN_SELCHANGED:
        if (idFrom == ctl1)
        {
            lParam = TV_GetParam(hwndTV);
            SetDlgItemInt(hwnd, stc1, (UINT)lParam, TRUE);
            printf("#%d was selected\n", lParam);
            fflush(stdout);
        }
        break;
    case TVN_DELETEITEM:
        if (idFrom == ctl1)
        {
            pTreeView = (NM_TREEVIEW *)pnmhdr;
            printf("#%d was deleted\n", pTreeView->itemOld.lParam);
            fflush(stdout);
        }
        break;
    case TVN_BEGINLABELEDIT:
        if (idFrom == ctl1)
        {
            pDispInfo = (TV_DISPINFO *)pnmhdr;
            if (TRUE)
            {
                printf("#%d label edit begins \n", pDispInfo->item.lParam);
                return TRUE;
            }
        }
        break;
    case TVN_ENDLABELEDIT:
        if (idFrom == ctl1)
        {
            pDispInfo = (TV_DISPINFO *)pnmhdr;
            if (TRUE)
            {
                if (pDispInfo->item.pszText)
                {
                    TreeView_SetItem(pnmhdr->hwndFrom, &pDispInfo->item);
                    printf("#%d label edit finished\n", pDispInfo->item.lParam);
                    return TRUE;
                }
                else
                {
                    printf("#%d label edit cancelled\n", pDispInfo->item.lParam);
                }
            }
        }
        break;
    }
    return 0;
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_NOTIFY, OnNotify);
    }
    return 0;
}

int main(void)
{
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(1), NULL, DialogProc);
    return 0;
}
